#include "Stopwatch.h"
#include "GraphicsSys.h"
#include "Configs.h"
#include <thread>
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#include <conio.h>

#include "Sprite.h"

#include "SequenceM.h"
#include "NumberGenerator.h"
#include "Transform.h"
#include "Color.h"
#include "Camera.h"
#include "Scripts.h"
#include "QuadTree.h"

#include <iostream>
/*

1) do a hash map data structure for reasons
2) get back to quad tree reenvigorated
3) make engine class for on user create and run loop sepparation
4) entities??? probably not

*/

int main() {

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    {
        using namespace badEngine;

        //configs
        Configs windowConfig("SystemManagerConfig.json");

        //init SDL system, can throw
        GraphicsSys renManager(windowConfig.get());

        ////// TEST CODE

        std::unique_ptr<StaticTexture> txtr = std::make_unique<StaticTexture>("C:/Users/ADMIN/Desktop/recomp/Textures/player_sheet_2.png", renManager);
        std::unique_ptr<Animation> anim = std::make_unique<Animation>(*txtr.get(), 32, 32);


        std::unique_ptr<TargetTexture> ttxtr = std::make_unique<TargetTexture>(960, 540, renManager);
        std::unique_ptr<Canvas> ctxtr = std::make_unique<Canvas>(*ttxtr.get());
        struct SomeObjWithArea {
            rectF rect;
            vec2f vel;
            Color col;
        };
        NumberGenerator rng;
        float windowWidth = 960;
        float windowHeight = 540;
        const rectI window(0, 0, windowWidth, windowHeight);

        QuadTree<SomeObjWithArea> myObjsQuad(rectF(0, 0, windowWidth, windowHeight));

        for (int i = 0; i < 10000; i++) {
            //ALSO TEST OUT WITH SMALLER RANGES TO TEST IF contains() is worth it for collision
            float boxWidth = rng.random_float(1, 10);
            float boxHeight = rng.random_float(1, 10);

            rectF box = rectF(rng.random_float(0, windowWidth - boxWidth), rng.random_float(0, windowHeight - boxHeight), boxWidth, boxHeight);
            SomeObjWithArea item = SomeObjWithArea(
                box,
                vec2f(rng.random_float(-1, 1), rng.random_float(-1, 1)),
                Color(rng.random_int(1, 255), rng.random_int(1, 255), rng.random_int(1, 255), 255)
            );

            myObjsQuad.insert(std::move(item), box);
        }

        Camera2D camera(960, 540);

        float mouseBoxSize = 50.0f;
        bool mouseHeld = false;


        bool plzDeleteArea = false;
        bool plzPruneMe = false;
        renManager.set_render_blend_mode(SDL_BLENDMODE_BLEND);

        float testMeAverage = 0.0f;
        std::size_t framesDone = 0;
        //////#######################################################

        //main loop
        Stopwatch UPDATE_DELTA_TIMER;
        bool GAME_RUNNING = true;
        SDL_Event EVENT;

        while (GAME_RUNNING) {
            static float accumulatedTime = 0;
            static const float frameTime = 0.008f;
            float dt = UPDATE_DELTA_TIMER.dt_float();
            accumulatedTime += dt;

            if (accumulatedTime >= frameTime) {

                //CLEAR RENDERING
                renManager.renderer_refresh();

                //LISTEN TO EVENTS

                while (SDL_PollEvent(&EVENT)) {
                    if (EVENT.type == SDL_EVENT_QUIT) {
                        GAME_RUNNING = false;
                        continue;
                    }

                    ////// TEST CODE
                    if (EVENT.key.key == SDLK_A) {
                        ctxtr->start_drawing(renManager);
                    }
                    ////////######################################################
                }

                //////TEST CODE        
                rectF cameraSpace = camera.get_view_rect();
                std::size_t objectsCount = 0;
                Stopwatch timer;
                //draw
                auto foundObjects = myObjsQuad.search_area(cameraSpace);
                for (const auto& each : foundObjects) {
                    //first draw
                    auto& obj = myObjsQuad[each];
                    rectF cameraAdjusted = camera.world_to_screen(obj.rect);
                    renManager.fill_area_with(cameraAdjusted, obj.col);
                    objectsCount++;
                }
                //apply move
                for (auto& each : foundObjects) {
                    auto& object = myObjsQuad[each];
                    rectF newPos(object.rect.x + object.vel.x, object.rect.y + object.vel.y, object.rect.w, object.rect.h);

                    //myObjsQuad.relocate(each, newPos);
                    //object.rect = newPos;
                }

                SequenceM<std::pair<std::size_t, rectF>> relocations;

                for (std::size_t i = 0; i < myObjsQuad.size(); ++i) {
                    auto& obj = myObjsQuad[i];
                    rectF newBox = obj.rect;
                    vec2f newVel = obj.vel;
                    newBox.increment_pos(newVel);

                    if (newBox.x < 0) {
                        newBox.x = 0;
                        newVel.x = -newVel.x;
                    }
                    else if (newBox.x + newBox.w >= 960) {
                        newBox.x = 960 - newBox.w;
                        newVel.x = -newVel.x;
                    }

                    if (newBox.y < 0) {
                        newBox.y = 0;
                        newVel.y = -newVel.y;
                    }
                    else if (newBox.y + newBox.h >= 540) {
                        newBox.y = 540 - newBox.h;
                        newVel.y = -newVel.y;
                    }
                    obj.rect = newBox;
                    obj.vel = newVel;
                    relocations.emplace_back(i, std::move(newBox));
                }

                auto colliders = myObjsQuad.search_collisions();
                myObjsQuad.relocate(relocations);

                static int justSomeCounter = 0;
                if (justSomeCounter == 10) {
                    myObjsQuad.remove_dead_cells();
                    justSomeCounter = 0;
                }
                justSomeCounter++;

                float elapsedTime = timer.dt_float();
                testMeAverage += elapsedTime;
                framesDone++;

                //draw mouse
                vec2f mouseScreenPos;
                SDL_GetMouseState(&mouseScreenPos.x, &mouseScreenPos.y);
                vec2f screenPos = camera.screen_to_world_point(mouseScreenPos);
                rectF rectAroundMouse = rectF(
                    screenPos.x - mouseBoxSize / 2,
                    screenPos.y - mouseBoxSize / 2,
                    mouseBoxSize, mouseBoxSize
                );
                rectF  cameraAdjustedMouse = camera.world_to_screen(rectAroundMouse);

                if (plzDeleteArea) {
                    rectF hardcode(200, 200, 200, 200);

                    myObjsQuad.remove_area(rectAroundMouse);
                }

                Color mouseCol = Colors::Magenta;
                mouseCol.set_alpha(125u);
                renManager.fill_area_with(cameraAdjustedMouse, mouseCol);



                //draw text
                std::string print = "Object Count: " + std::to_string(objectsCount) + "/" + std::to_string(myObjsQuad.size()) + "\ntime: " + std::to_string(elapsedTime) + "\nnodes: " + std::to_string(myObjsQuad.branch_count());



                float x, y;
                SDL_GetMouseState(&x, &y);
                vec2f mpos(x, y);
                anim->anim_update(accumulatedTime, &mpos);
                renManager.draw(anim->get_texture(), anim->get_source(), anim->get_dest());

                ctxtr->end_drawing(renManager);

                renManager.draw(ctxtr->get_texture(), ctxtr->get_source(), ctxtr->get_dest());

                //////########################################################

                //PRESENT
                renManager.renderer_present();
                accumulatedTime -= frameTime;
            }
        }
    }
    _CrtDumpMemoryLeaks();
    return 0;
}



/*
 struct SomeObjWithArea {
        rectF rect;
        vec2f vel;
        Color col;
    };
    NumberGenerator rng;
    float windowWidth = 960;
    float windowHeight = 540;
    const rectI window(0, 0, windowWidth, windowHeight);

    QuadTree<SomeObjWithArea> myObjsQuad(rectF(0, 0, windowWidth, windowHeight));

    for (int i = 0; i < 10000; i++) {
        //ALSO TEST OUT WITH SMALLER RANGES TO TEST IF contains() is worth it for collision
        float boxWidth = rng.random_float(1, 10);
        float boxHeight = rng.random_float(1, 10);

        rectF box = rectF(rng.random_float(0, windowWidth - boxWidth), rng.random_float(0, windowHeight - boxHeight), boxWidth, boxHeight);
        SomeObjWithArea item = SomeObjWithArea(
            box,
            vec2f(rng.random_float(-1, 1), rng.random_float(-1, 1)),
            Color(rng.random_int(1, 255), rng.random_int(1, 255), rng.random_int(1, 255), 255)
        );

        myObjsQuad.insert(std::move(item), box);
    }

    Camera2D camera(960, 540);

    float mouseBoxSize = 50.0f;
    bool mouseHeld = false;

    std::unique_ptr<Texture> fontTextureOwner = std::make_unique<Texture>("C:/Users/ADMIN/Desktop/recomp/Fonts/font_32x3.png", renManager);
    std::unique_ptr<Font> font = std::make_unique<Font>(*fontTextureOwner.get(), 32, 3);


    bool plzDeleteArea = false;
    bool plzPruneMe = false;
    renManager.set_render_blend_mode(SDL_BLENDMODE_BLEND);

    float testMeAverage = 0.0f;
    std::size_t framesDone = 0;
*/

/*
            if (EVENT.key.key == SDLK_A) {
                mouseBoxSize += 10.0f;
            }
            if (EVENT.key.key == SDLK_S) {
                mouseBoxSize -= 10.0f;
            }
            if (EVENT.key.key == SDLK_P) {
                plzPruneMe = true;
            }
            if (EVENT.type == SDL_EVENT_MOUSE_BUTTON_DOWN && EVENT.button.button == SDL_BUTTON_LEFT) {
                plzDeleteArea = true;
            }
            if (EVENT.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                plzDeleteArea = false;
            }
            script_handle_camera_mouse(EVENT, camera);
*/

/*
        rectF cameraSpace = camera.get_view_rect();
        std::size_t objectsCount = 0;
        Stopwatch timer;
        //draw
        auto foundObjects = myObjsQuad.search_area(cameraSpace);
        for (const auto& each : foundObjects) {
            //first draw
            auto& obj = myObjsQuad[each];
            rectF cameraAdjusted = camera.world_to_screen(obj.rect);
            renManager.fill_area_with(cameraAdjusted, obj.col);
            objectsCount++;
        }
        //apply move
        for (auto& each : foundObjects) {
            auto& object = myObjsQuad[each];
            rectF newPos(object.rect.x + object.vel.x, object.rect.y + object.vel.y, object.rect.w, object.rect.h);

            //myObjsQuad.relocate(each, newPos);
            //object.rect = newPos;
        }

        SequenceM<std::pair<std::size_t, rectF>> relocations;

        for (std::size_t i = 0; i < myObjsQuad.size(); ++i) {
            auto& obj = myObjsQuad[i];
            rectF newBox = obj.rect;
            vec2f newVel = obj.vel;
            newBox.increment_pos(newVel);

            if (newBox.x < 0) {
                newBox.x = 0;
                newVel.x = -newVel.x;
            }
            else if (newBox.x + newBox.w >= 960) {
                newBox.x = 960 - newBox.w;
                newVel.x = -newVel.x;
            }

            if (newBox.y < 0) {
                newBox.y = 0;
                newVel.y = -newVel.y;
            }
            else if (newBox.y + newBox.h >= 540) {
                newBox.y = 540 - newBox.h;
                newVel.y = -newVel.y;
            }
            obj.rect = newBox;
            obj.vel = newVel;
            relocations.emplace_back(i, std::move(newBox));
        }

        auto colliders = myObjsQuad.search_collisions();
        myObjsQuad.relocate(relocations);

        static int justSomeCounter = 0;
        if (justSomeCounter == 10) {
            myObjsQuad.remove_dead_cells();
            justSomeCounter = 0;
        }
        justSomeCounter++;

        float elapsedTime = timer.dt_float();
        testMeAverage += elapsedTime;
        framesDone++;

        //draw mouse
        vec2f mouseScreenPos;
        SDL_GetMouseState(&mouseScreenPos.x, &mouseScreenPos.y);
        vec2f screenPos = camera.screen_to_world_point(mouseScreenPos);
        rectF rectAroundMouse = rectF(
            screenPos.x - mouseBoxSize / 2,
            screenPos.y - mouseBoxSize / 2,
            mouseBoxSize, mouseBoxSize
        );
        rectF  cameraAdjustedMouse = camera.world_to_screen(rectAroundMouse);

        if (plzDeleteArea) {
            rectF hardcode(200, 200, 200, 200);

            myObjsQuad.remove_area(rectAroundMouse);
        }

        Color mouseCol = Colors::Magenta;
        mouseCol.set_alpha(125u);
        renManager.fill_area_with(cameraAdjustedMouse, mouseCol);



        //draw text
        std::string print = "Object Count: " + std::to_string(objectsCount) + "/" + std::to_string(myObjsQuad.size()) + "\ntime: " + std::to_string(elapsedTime) + "\nnodes: " + std::to_string(myObjsQuad.branch_count());

        font->set_text(print, vec2f(0, 0));
        renManager.draw(font->get_texture(), font->get_letter_positions());
*/