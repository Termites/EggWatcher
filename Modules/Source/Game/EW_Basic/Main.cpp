#include <Core/Character.hpp>
<<<<<<< HEAD
#include <Graphic/Window.hpp>
#include <Graphic/Shader.hpp>
#include <Graphic/Mesh/StaticMesh.hpp>
#include <Graphic/Mesh/SkeletalMesh.hpp>
#include <Graphic/Scene.hpp>
#include <Graphic/DynamicMeshInstance.hpp>
=======
#include <Network/Request.hpp>
>>>>>>> 516f42b01c75dfe6c44a2cc967f6000942d1bd3e
#include <iostream>

/*
    NASTY NASTY NASTY NOMNOM
*/

class Hunger_Need : public Core::Need
{
        protected:
            float UpdateFactor;

        public:
            Hunger_Need() : Core::Need()
            {
                UpdateFactor = 0.12;
            }

            virtual Core::EmotionModifier Update()
            {
                Factor += UpdateFactor;
                Core::EmotionModifier EM;
                EM.first = Core::EmotionEnum::Sad;

                EM.second = Factor * 0.2;

                return EM;
            }
};

class MyCharacter : public Core::Character
{
    public:
        MyCharacter() : Core::Character()
        {

        }
        virtual void OnEmotionChange(Core::EmotionEnum NewEmotion)
        {
            std::cout<<"\nMy emotion changed:\n\tI was "<<Core::EmotionToString(Emotion)<<", and i'm now "<<Core::EmotionToString(NewEmotion)<<std::endl;
            DumpEmotionnalState();
        }

        void DumpEmotionnalState()
        {
            std::cout<<"---------------------------------"<<std::endl;
            std::cout<<"Current Emotionnal State"<<std::endl;
            std::cout<<"Fear : "<<FearState.second<<std::endl;
            std::cout<<"Anger : "<<AngryState.second<<std::endl;
            std::cout<<"Sadness: "<<SadState.second<<std::endl;
            std::cout<<"Happyness : "<<JoyState.second<<std::endl;
            std::cout<<"---------------------------------"<<std::endl;
        }
};

/*
    Needs:
        Eat
        Drink
        Go Out
        Play
        Sleep
*/

int main()
{
    //=======================================================
    // Initialize Graphic Module
    Graphic::Window W;
    W.Create("EggWatcher - Early Indev - ",800,600,32,false);

    GLenum E = glewInit();

    if (E != GLEW_OK)
    {
        std::cout<<"Error while initializing GLEW : ";
        std::cout<<glewGetErrorString(E)<<std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //=======================================================

    //=======================================================
    // Create and setup Scene
    Graphic::Camera C;

    C.SetLocation(glm::vec3(12,-12,8.4));
    C.SetLookAt(glm::vec3(0.0,0.0,5.935));

    Graphic::Scene S;

    Graphic::SkeletalMesh_Ptr SKM(new Graphic::SkeletalMesh);
    SKM->LoadFromFile("FormA.edm");

    Graphic::EKPPackage_Ptr PCK(new Graphic::EKPPackage);
    PCK->LoadFromFile("FormAPackage.ekp");

    SKM->SetPackage(PCK);

    Graphic::Texture2D_Ptr Tex(new Graphic::Texture2D);
    Tex->LoadFromFile("FormADiff.png");

    Graphic::Texture2D_Ptr HeadTex(new Graphic::Texture2D);
    HeadTex->LoadFromFile("Faces/fA_Open.png");

    Graphic::Texture2D_Ptr HeadTexB(new Graphic::Texture2D);
    HeadTexB->LoadFromFile("Faces/fA_Closed.png");


    Graphic::DynamicMeshInstance_Ptr NewMIP(new Graphic::DynamicMeshInstance);
    NewMIP->SetTexture(Graphic::TextureSlot::Diffuse, Tex);
    NewMIP->SetTexture(Graphic::TextureSlot::Overlay0, HeadTex);
    NewMIP->SetOverlayOffsetSize(0,glm::vec4(-2.73,-2.25,4.0,4.0));
    NewMIP->PlayAnimation(0,0.75f);
    S.AddMeshInstance(NewMIP);
    NewMIP->SetDynamicReference(SKM);


    S.UpdatePerspective(60.f, 800.0 / 600.0);

    S.SetCamera(C);



    {
        Graphic::Texture2D_Ptr TexB(new Graphic::Texture2D);
        TexB->LoadFromFile("FloorTex.png");

        Graphic::StaticMesh_Ptr SM(new Graphic::StaticMesh);
        SM->LoadFromFile("Floor.obj");
        //for (int i=-8;i<8;++i)
        {
            //for (int j=-8;j<8;++j)
            {
                Graphic::MeshInstance_Ptr NewMIP (new Graphic::MeshInstance);

                //NewMIP->SetLocation(glm::vec3(i * 16, j* 16, 0));
                NewMIP->SetTexture(Graphic::TextureSlot::Diffuse, TexB);

                NewMIP->SetReferenceMesh(SM);
                S.AddMeshInstance(NewMIP);
            }
        }
    }

    S.SetLightDirection(glm::vec3(6,1,8));

    //=======================================================

    //=======================================================
    // Main Loop
    bool isRunning = true;

    W.Start();

    float XX = 20.0f;
    float YY = 00.0f;
    int Dir = 0;
    int yDir = 0;

    glm::vec3 Pos = glm::vec3(-20,60,0);

    int EyeState = 0;
    float BlinkTime = 4000.0;

    float SunAngle = 5;

    glm::vec3 SunColor;

    // Color chart :
    // <0> : lt blue
    // <90> lt yellow
    // <180> lt blue

    S.SetLightColor(glm::vec3(99,118,149)/255.f);

    float TimeDt = 0.0;

    glm::vec3 SkyInitColor = glm::vec3(194,206,224) / 255.f;
    glm::vec3 SkyMaxColor = glm::vec3(255,250,224) / 255.f;
    glm::vec3 SkyMinColor = glm::vec3(125,130,136) / 255.f;

    while (isRunning)
    {
        Graphic::WindowEvent E;
        while (W.GetEvent(E))
        {
            switch (E.Type)
            {
                case Graphic::WinEventType::Closed:
                    isRunning = false;
                break;

                case Graphic::WinEventType::Resized:
                    S.Resize(E.Width,E.Height);
                break;

                case Graphic::WinEventType::KeyPressed:
                    if (E.KeyCode == Graphic::Input::KeyCode::Left)
                        Dir = 1;
                    else if (E.KeyCode == Graphic::Input::KeyCode::Right )
                        Dir = -1;
                    if (E.KeyCode == Graphic::Input::KeyCode::Up)
                        yDir = 1;
                    else if (E.KeyCode == Graphic::Input::KeyCode::Down)
                        yDir = -1;
                break;

                case Graphic::WinEventType::KeyReleased:
                    Dir = 0;
                    yDir = 0;
                break;
            }
        }

        glm::vec3 SunColor;

        TimeDt += W.GetFrameTime()* 0.0001f;

        float TimeSinCtrl = glm::sin(TimeDt * 2 * 3.141569);

        if (TimeSinCtrl > 0.0)
        {
            float Delta = TimeSinCtrl;
            // Fade to day
            SunColor = SkyInitColor * (1.0f - Delta) + SkyMaxColor * Delta;
        }
        else
        {
            float Delta = -TimeSinCtrl;
            // Fade to day
            SunColor = SkyInitColor * (1.0f - Delta) + SkyMinColor * Delta;
        }

        glm::vec3 SunPos;

        SunPos.x = (glm::cos(TimeDt * 2 * 3.141569));
        SunPos.z = glm::abs(glm::sin(TimeDt * 2 * 3.141569));
        if (SunPos.z < 1.0)
            SunPos.z = 1.0;

        S.SetLightColor(SunColor);
        S.SetLightDirection(SunPos);


        BlinkTime -= W.GetFrameTime();
        if (BlinkTime < 0)
        {
            if (EyeState == 0) {
                NewMIP->SetTexture(Graphic::TextureSlot::Overlay0, HeadTexB);
                BlinkTime = 100;
            }
            else
            {
                NewMIP->SetTexture(Graphic::TextureSlot::Overlay0, HeadTex);
                BlinkTime = 3000;
            }
            EyeState = 1 - EyeState;
        }


        XX += Dir * W.GetFrameTime() * 0.2;
        YY += yDir * W.GetFrameTime() * 0.2;

        Pos += -glm::vec3( glm::sin(glm::radians(-XX)), glm::cos(glm::radians(-XX)), 0.0f) * float ( W.GetFrameTime() ) * 0.005f;

        NewMIP->SetOrientation(glm::vec3(YY,0,XX));
        //NewMIP->SetLocation(Pos);

        S.Tick(W.GetFrameTime());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(SunColor.x,SunColor.y,SunColor.z,0.0);
        S.Draw();

        W.Refresh();

        Sleep(1);
    }
    //=======================================================

    return 0;
}

int T_main(int arg_c, char* arg_v[])
{
    Graphic::Window W;
    W.Create("EggWatcher - Early Indev - ",800,600,32,false);
    MyCharacter C;
    C.AddNeed(new Hunger_Need);

    Core::PersonnalityModifier Personnality;

	Personnality.FearFactor = 0.2f;
	Personnality.AngryFactor = 0.6f;
	Personnality.SadFactor = 0.18f;
	Personnality.JoyFactor = 0.9f;
	Personnality.SadToAngryFactor = 0.018f;
	Personnality.FearToAngryFactor = 0.001f;
	Personnality.LunaticFactor = 0.48f;

	C.SetPersonnality(Personnality);

    for (int i=0;i<1000;++i)
    {
        std::cout<<'.';
       // std::cout<<"Iteration # "<<i+1<<std::endl;
        C.Update();
    }

    std::cout<<std::endl;

    C.DumpEmotionnalState();

    bool isRunning = true;

    while (isRunning)
    {
        Graphic::WindowEvent E;
        while (W.GetEvent(E))
        {
            switch (E.Type)
            {
                case Graphic::WinEventType::Closed:
                    isRunning = false;
                break;
            }
        }
    }

    return 0;
}
