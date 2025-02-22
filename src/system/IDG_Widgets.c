#include "../IDG_Common.h"

#include "../system/IDG_Draw.h"
#include "../system/IDG_Sound.h"
#include "../system/IDG_Text.h"
#include "../system/IDG_Util.h"
#include "../json/cJSON.h"

#include "IDG_Widgets.h"

extern app_t app;

static widget_t  widget_head;
static widget_t *widget_tail;

static void IDG_CreateWidget        (cJSON *root);
static int  IDG_GetWidgetType       (char *type);

static void IDG_CreateButtonWidget  (widget_t *w, cJSON *root);
static void IDG_CreateSelectWidget  (widget_t *w, cJSON *root);
static void IDG_CreateSliderWidget  (widget_t *w, cJSON *root);
static void IDG_CreateInputWidget   (widget_t *w, cJSON *root);
static void IDG_CreateControlWidget (widget_t *w, cJSON *root);

static void IDG_DrawButtonWidget    (widget_t *w);
static void IDG_DrawSelectWidget    (widget_t *w);
static void IDG_DrawSliderWidget    (widget_t *w);
static void IDG_DrawInputWidget     (widget_t *w);
static void IDG_DrawControlWidget   (widget_t *w);

static void IDG_DoInputWidget       (void);
static void IDG_DoControlWidget     (void);

static void IDG_ChangeWidgetValue   (int dir);

static void IDG_LoadWidgets         (char *filename);

static double slider_delay;
static double cursor_blink;
static int    handle_input_widget;
static int    handle_control_widget;

void IDG_InitWidgets(void)
{
    memset(&widget_head, 0, sizeof(widget_t));
    widget_tail = &widget_head;

    // *** TODO ***
    // Include/load project-specific widget files here.
    // Better yet - scan this from a fuckin directory or something.
    
    // IDG_LoadWidgets("data/widgets/options.json");
    IDG_LoadWidgets("data/widgets/sectors.json");

    slider_delay          = 0;
    cursor_blink          = 0;
    handle_input_widget   = 0;
    handle_control_widget = 0;
}

void IDG_DoWidgets(char *group_name)
{
    slider_delay  = MAX((slider_delay - app.delta_time), 0);
    cursor_blink += app.delta_time;

    if(!handle_input_widget && !handle_control_widget)
    {
        if(app.keyboard[SDL_SCANCODE_UP])
        {
            app.keyboard[SDL_SCANCODE_UP] = 0;

            do // Only process widgets in the current group
            {
                app.active_widget = app.active_widget->prev;
                if(app.active_widget == &widget_head)
                    app.active_widget = widget_tail;
            } while(strcmp(app.active_widget->group_name, group_name) != 0);
            
            // play_sound(SND_GUI, 0);
        }

        if(app.keyboard[SDL_SCANCODE_DOWN])
        {
            app.keyboard[SDL_SCANCODE_DOWN] = 0;

            do // Only process widgets in the current group
            {
                app.active_widget = app.active_widget->next;

                if(app.active_widget == NULL)
                    app.active_widget = widget_head.next;
            } while(strcmp(app.active_widget->group_name, group_name) != 0);
            
            // play_sound(SND_GUI, 0);
        }

        if(app.keyboard[SDL_SCANCODE_LEFT])
        {
            app.keyboard[SDL_SCANCODE_LEFT] = 0;
            IDG_ChangeWidgetValue(-1);
        }

        if(app.keyboard[SDL_SCANCODE_RIGHT])
        {
            app.keyboard[SDL_SCANCODE_RIGHT] = 0;
            IDG_ChangeWidgetValue(1);
        }

        if(app.keyboard[SDL_SCANCODE_SPACE] || app.keyboard[SDL_SCANCODE_RETURN])
        {
            app.keyboard[SDL_SCANCODE_SPACE] = app.keyboard[SDL_SCANCODE_RETURN] = 0;

            if(app.active_widget->type == WT_INPUT)
            {
                cursor_blink        = 0;
                handle_input_widget = 1;
                memset(app.input_text, 0, sizeof(app.input_text));
            } 
            else if(app.active_widget->type == WT_CONTROL)
            {
                app.last_key_pressed    = -1;
                app.last_button_pressed = -1;
                handle_control_widget   = 1;
            }
            else if(app.active_widget->action != NULL)
            {
                app.active_widget->action();
            }

            // play_sound(SND_GUI, 0);
        }
    }
    else if(handle_input_widget)
        IDG_DoInputWidget();
    else if(handle_control_widget)
        IDG_DoControlWidget();
}

static void IDG_ChangeWidgetValue(int dir)
{
    select_widget_t *select;
    slider_widget_t *slider;

    switch(app.active_widget->type)
    {
        case WT_SELECT:
            select = (select_widget_t *)app.active_widget->data;
            select->value += dir;

            if(select->value < 0)
                select->value = select->num_options - 1;
            if(select->value >= select->num_options)
                select->value = 0;

            if(app.active_widget->action != NULL)
                app.active_widget->action();
            
            // play_sound(SND_GUI, 0);
            break;
        case WT_SLIDER:
            slider = (slider_widget_t *)app.active_widget->data;

            if(slider_delay == 0 || slider->wait_on_change)
            {
                if(slider->wait_on_change)
                {
                    app.keyboard[SDL_SCANCODE_LEFT] = app.keyboard[SDL_SCANCODE_RIGHT] = 0;
                    // play_sound(SND_GUI, 0);
                }
                
                slider->value = MIN(MAX(slider->value + (slider->step * dir), 0), 100);
                slider_delay  = 1;

                if(app.active_widget->action != NULL)
                    app.active_widget->action();
            }
            break;
        default:
            break;
    }
}

static void IDG_DoInputWidget(void)
{
    input_widget_t *iw;
    int             i, l, n;

    iw = (input_widget_t *)app.active_widget->data;

    l = strlen(iw->text);
    n = strlen(app.input_text);

    if((n+1) > iw->max_length)
        n = (iw->max_length-1);
    
    for(i=0; i<n; i++)
        iw->text[l++] = app.input_text[i];
    
    memset(app.input_text, 0, sizeof(app.input_text));

    if(l > 0 && app.keyboard[SDL_SCANCODE_BACKSPACE])
    {
        iw->text[--l] = '\0';
        app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;
    }

    if(app.keyboard[SDL_SCANCODE_RETURN] || app.keyboard[SDL_SCANCODE_ESCAPE])
    {
        app.keyboard[SDL_SCANCODE_RETURN] = app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
        handle_input_widget = 0;

        if(app.active_widget->action != NULL)
            app.active_widget->action();
        
        // play_sound(SND_GUI, 0);
    }
}

static void IDG_DoControlWidget(void)
{
    control_widget_t *cw;

    if(app.last_key_pressed != -1 || app.last_button_pressed != -1)
    {
        cw = (control_widget_t *)app.active_widget->data;

        if(app.last_key_pressed == SDL_SCANCODE_BACKSPACE)
        {
            cw->keyboard = 0;
            cw->joypad   = -1;
        }
        else if(app.last_key_pressed != SDL_SCANCODE_ESCAPE)
        {
            if(app.last_key_pressed != -1)
            {
                cw->keyboard = app.last_key_pressed;
                app.keyboard[app.last_key_pressed] = 0;
            }

            if(app.last_button_pressed != -1)
            {
                cw->joypad = app.last_button_pressed;
                app.joypad_button[app.last_button_pressed] = 0;
            }

            if(app.active_widget->action != NULL)
                app.active_widget->action();
        }
    
        handle_control_widget = 0;
    }
}

void IDG_DrawWidgets(char *group_name)
{
    widget_t *w;
    int       h;

    for(w=widget_head.next; w!=NULL; w=w->next)
    {
        if(strcmp(w->group_name, group_name) == 0) // Only process widgets in the current group
        {
            switch(w->type)
            {
                case WT_BUTTON:
                    IDG_DrawButtonWidget  (w);
                    break;
                case WT_SELECT:
                    IDG_DrawSelectWidget  (w);
                    break;
                case WT_SLIDER:
                    IDG_DrawSliderWidget  (w);
                    break;
                case WT_INPUT:
                    IDG_DrawInputWidget   (w);
                    break;
                case WT_CONTROL:
                    IDG_DrawControlWidget (w);
                    break;
                default:
                    break;
            }

            if(w == app.active_widget)
            {
                h = w->h/2;
                IDG_DrawRect(w->x-(h*2), w->y+(h/2), h, h, 0, 255, 0, 255);
            }
        }
    }
}

static void IDG_DrawButtonWidget(widget_t *w)
{
    SDL_Color c;

    if(w == app.active_widget)
    {
        c.g = 255;
        c.r = c.b = 0;
    }
    else
    {
        c.r = c.g = c.b = 255;
    }

    IDG_DrawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
}

static void IDG_DrawSelectWidget(widget_t *w)
{
    select_widget_t *s;
    SDL_Color        c;
    char             text[128];

    s = (select_widget_t *)w->data;

    if(w == app.active_widget)
    {
        c.g = 255;
        c.r = c.b = 0;
    }
    else
    {
        c.r = c.g = c.b = 255;
    }

    IDG_DrawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
    sprintf(text, "< %s >", s->options[s->value]);
    IDG_DrawText(text, s->x, s->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
}

static void IDG_DrawSliderWidget(widget_t *w)
{
    slider_widget_t *s;
    SDL_Color        c;
    double           width;

    s = (slider_widget_t *)w->data;

    if(w == app.active_widget)
    {
        c.g = 255;
        c.r = c.b = 0;
    }
    else
    {
        c.r = c.g = c.b = 255;
    }

    width = (1.0*s->value)/100;
    IDG_DrawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
    IDG_DrawRect((s->x+2), (s->y+2), (s->w-4)*width, (s->h-4), c.r, c.g, c.b, 255);
    IDG_DrawOutlineRect(s->x, s->y, s->w, s->h, 255, 255, 255, 255);
}

static void IDG_DrawInputWidget(widget_t *w)
{
    input_widget_t *i;
    SDL_Color       c;
    int             width, height;

    i = (input_widget_t *)w->data;

    if(w == app.active_widget)
    {
        c.g = 255;
        c.r = c.b = 0;
    }
    else
    {
        c.r = c.g = c.b = 255;
    }

    IDG_DrawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
    IDG_DrawText(i->text, i->x, i->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);

    if(handle_input_widget && app.active_widget == w && ((int)cursor_blink % (int)FPS < (FPS/2)))
    {
        IDG_GetTextDimensions(i->text, &width, &height);
        IDG_DrawRect((i->x+width+4), (i->y+14), 32, 32, 0, 255, 0, 255);
    }
}

static void IDG_DrawControlWidget(widget_t *w)
{
    control_widget_t *cw;
    SDL_Color         c;
    char              text[32];

    cw = (control_widget_t *)w->data;

    if(w == app.active_widget)
    {
        c.g = 255;
        c.r = c.b = 0;
    }
    else
    {
        c.r = c.g = c.b = 255;
    }

    IDG_DrawText(w->label, w->x, w->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);

    if(handle_control_widget && app.active_widget == w)
    {
        IDG_DrawText("...", cw->x, cw->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
    }
    else
    {
        if(cw->keyboard != 0 && cw->joypad != -1)
            sprintf(text, "%s or Btn %d", SDL_GetScancodeName(cw->keyboard), cw->joypad);
        else if(cw->keyboard != 0)
            sprintf(text, "%s", SDL_GetScancodeName(cw->keyboard));
        else if(cw->joypad != -1)
            sprintf(text, "Btn %d", cw->joypad);
        else
            sprintf(text, "N/A");

        IDG_DrawText(text, cw->x, cw->y, c.r, c.g, c.b, TEXT_ALIGN_LEFT, 0);
    }
}

widget_t *IDG_GetWidget(char *name, char *group_name)
{
    widget_t *w;

    for(w=widget_head.next; w!=NULL; w=w->next)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Widget found: '%s', %s ...", name, group_name);
        if(strcmp(w->name, name) == 0 && strcmp(w->group_name, group_name) == 0)
            return w;
    }
    
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "No such widget: '%s'\n", name);
    return NULL;
}

static void IDG_LoadWidgets(char *filename)
{
    cJSON *root, *node;
    char  *text;

    text = IDG_ReadFile (filename);
    root = cJSON_Parse  (text);
    for(node=root->child; node!=NULL; node=node->next)
        IDG_CreateWidget(node);
    
    cJSON_Delete (root);
    free         (text);
}

static void IDG_CreateWidget(cJSON *root)
{
    widget_t *w;
    int       type;

    type = IDG_GetWidgetType(cJSON_GetObjectItem(root, "type")->valuestring);
    if(type != -1)
    {
        w = malloc(sizeof(widget_t));
        memset(w, 0, sizeof(widget_t));
        widget_tail->next = w;
        w->prev           = widget_tail;
        widget_tail       = w;

        STRCPY(w->name,       cJSON_GetObjectItem(root, "name"     )->valuestring);
        STRCPY(w->label,      cJSON_GetObjectItem(root, "label"    )->valuestring);
        STRCPY(w->group_name, cJSON_GetObjectItem(root, "group_name")->valuestring);

        w->type = IDG_GetWidgetType(cJSON_GetObjectItem(root, "type")->valuestring);
        w->x    = cJSON_GetObjectItem(root, "x")->valueint;
        w->y    = cJSON_GetObjectItem(root, "y")->valueint;

        switch(w->type)
        {
            case WT_BUTTON:
                IDG_CreateButtonWidget  (w, root);
                break;
            case WT_SELECT:
                IDG_CreateSelectWidget  (w, root);
                break;
            case WT_SLIDER:
                IDG_CreateSliderWidget  (w, root);
                break;
            case WT_INPUT:
                IDG_CreateInputWidget   (w, root);
                break;
            case WT_CONTROL:
                IDG_CreateControlWidget (w, root);
                break;
            default:
                break;
        }
    }
}

static void IDG_CreateButtonWidget(widget_t *w, cJSON *root)
{
    IDG_GetTextDimensions(w->label, &w->w, &w->h);
}

static void IDG_CreateSelectWidget(widget_t *w, cJSON *root)
{
    select_widget_t *s;
    cJSON           *options, *node;
    int              i, len;

    s = malloc(sizeof(select_widget_t));
    memset(s, 0, sizeof(select_widget_t));
    w->data = s;

    options = cJSON_GetObjectItem(root, "options");
    s->num_options = cJSON_GetArraySize(options);

    if(s->num_options > 0)
    {
        i = 0;
        s->options = malloc(sizeof(char *) * s->num_options);

        for(node=options->child; node!=NULL; node=node->next)
        {
            len = strlen(node->valuestring)+1;
            s->options[i] = malloc(len);
            STRNCPY(s->options[i], node->valuestring, len);
            i++;
        }
    }

    IDG_GetTextDimensions(w->label, &w->w, &w->h);
    s->x = w->x + w->w + 50;
    s->y = w->y;
}

static void IDG_CreateSliderWidget(widget_t *w, cJSON *root)
{
    slider_widget_t *s;
    s = malloc(sizeof(slider_widget_t));
    memset(s, 0, sizeof(slider_widget_t));
    w->data = s;

    s->step           = cJSON_GetObjectItem(root, "step")->valueint;
    s->wait_on_change = cJSON_GetObjectItem(root, "waitOnChange")->valueint;

    IDG_GetTextDimensions(w->label, &w->w, &w->h);
}

static void IDG_CreateInputWidget(widget_t *w, cJSON *root)
{
    input_widget_t *iw;

    iw = malloc(sizeof(input_widget_t));
    memset(iw, 0, sizeof(input_widget_t));
    w->data = iw;

    iw->max_length = cJSON_GetObjectItem(root, "maxLength")->valueint;
    iw->text       = malloc(iw->max_length+1);
    IDG_GetTextDimensions(w->label, &w->w, &w->h);
}

static void IDG_CreateControlWidget(widget_t *w, cJSON *root)
{
    control_widget_t *cw;
     
    cw = malloc(sizeof(control_widget_t));
    memset(cw, 0, sizeof(control_widget_t));
    w->data = cw;

    IDG_GetTextDimensions(w->label, &w->w, &w->h);
}

static int IDG_GetWidgetType(char *type)
{
    if(strcmp(type, "WT_BUTTON") == 0)
        return WT_BUTTON;
    if(strcmp(type, "WT_SELECT") == 0)
        return WT_SELECT;
    if(strcmp(type, "WT_SLIDER") == 0)
        return WT_SLIDER;
    if(strcmp(type, "WT_INPUT") == 0)
        return WT_INPUT;
    if(strcmp(type, "WT_CONTROL") == 0)
        return WT_CONTROL;
    
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Unknown widget type: '%s'\n", type);
    return -1;
}