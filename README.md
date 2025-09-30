    //// Description////
# STM32_GameRender_EngineGameObject
This is workest Game Engine for my Nucleo-STM32G071RB witch 64MHZ and 36KB, for now you can try this engine in CubeIDE for your STM32 witch TFT SPI, this engine based on redering automatic the all object sorting on Z Layer, user need to create solid rectagle sprite for objects or Bitmap costum sprite witch function in "gameobject.h" like use UNITY, You can help me to inprove this engine to work, or inprove for your self.
/////Info_needest_file////
1)in this project for CubeIDE, all file is in Core/Src and Core/Inc, and pin configuration for TFT SPI 240*320 pixels and control four buttons "BTN" in file .ioc.
2) All "EngineRendering" file in Core/Src/EngineC and Core/Inc/EngineH.
3) Game file in Core/Src/gameC and Core/Inc/gameH in this folders for now you have mario.c game for Exaples.
4) Library for TFT SPI 240*320 located in Core/Src and /Inc witch name fonts.h, ILI9341_STM32_Driver.h ILI9341_GFX.h and in Core/Src we have only ILI9341_STM32_Driver.c ILI9341_GFX.c for moment working witch DMA but not syncronyzed, i dont' know how to synkronyzed.
5)Yes this is all file incluses in cubeIDE project.

//////Support and future inprovments///// very inportant/////
1)For this moment i dont' have enouch Expirince to inprove trasffering DMA witch bloking date and buffering frame witch dirty rectagles before representate on display(for remove blinking of object). (all this in driver of TFT SPI)
2)Need to inprove and optimize Reder object algoritm for rendering only moved object or intersectional witch 2 object or more. (render.c and gameobject.c)


///Help witch some Coffe////
☕☕☕☕☕☕☕☕☕☕☕☕
itchi.io https://clainyofficial.itch.io/silksong-quizer
