/*
 * context.h (SDL2)
 *
 * This file is part of the "PicoRenderer" (Copyright (c) 2014 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef __PR_CONTEXT_H__
#define __PR_CONTEXT_H__


#include "types.h"
#include "framebuffer.h"
#include "color_palette.h"
#include "color.h"
#include "platform.h"
#include "state_machine.h"

#include <SDL2/SDL.h>


//! Render context structure.
typedef struct pr_context
{
    // SDL2 objects
    SDL_Window*              wnd;
    SDL_Renderer*            ren;
    SDL_Texture*             tex;

    // Renderer objects
    pr_color*           colors;
    PRuint              width;
    PRuint              height;
    pr_color_palette*   colorPalette;

    // State objects
    pr_state_machine    stateMachine;
}
pr_context;


extern pr_context* _currentContext;

//! Creates a new render context for the specified device context.
pr_context* _pr_context_create(const PRcontextdesc* desc, PRuint width, PRuint height);
//! Deletes the specified render context.
void _pr_context_delete(pr_context* context);

//! Makes the specified context to the current one.
void _pr_context_makecurrent(pr_context* context);

/**
Presents the specified framebuffer onto the render context.
Errors:
- PR_ERROR_NULL_POINTER : If 'context', 'framebuffer' or 'colorPalette' is null.
- PR_ERROR_ARGUMENT_MISMATCH : If 'context' has another dimension than 'framebuffer'.
*/
void _pr_context_present(pr_context* context, const pr_framebuffer* framebuffer);


#endif
