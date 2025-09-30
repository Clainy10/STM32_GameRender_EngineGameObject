#  STM32 Game Render Engine (GameObject)

This is a lightweight **Game Engine** for the **Nucleo-STM32G071RB** (64 MHz, 36 KB RAM).  
It runs in **STM32CubeIDE** and supports **TFT SPI 240×320 displays** (ILI9341 controller).  

The engine automatically renders all objects with **Z-Layer sorting**.  
You can create:
- 🟦 Solid rectangle sprites  
- 🖼️ Custom bitmap sprites (see `gameobject.h`)  

//Think of it as a **mini-Unity for STM32**//

---

## 📂 Project Structure

1. **Main CubeIDE files**  
   - Located in `Core/Src` and `Core/Inc`  
   - TFT SPI pins & 4 button inputs configured in the `.ioc` file  (user label BTN)

2. **Engine files**  
   - `Core/Src/EngineC`  
   - `Core/Inc/EngineH`  

3. **Game files (examples)**  
   - `Core/Src/gameC`  
   - `Core/Inc/gameH`  
   - Includes `mario.c` as a demo game 

4. **Display driver & fonts**  
   - `fonts.h`  
   - `ILI9341_STM32_Driver.h / .c`  
   - `ILI9341_GFX.h / .c`  
   - Currently works with **DMA** (not fully synchronized yet)  

---

## ⚠️ Current Limitations / Future Improvements

-  DMA transfer is blocking → need non-blocking + dirty rectangles  
-  Rendering algorithm could be optimized:  
  - Only redraw moved objects  
  - Handle object intersections more efficiently (`render.c`, `gameobject.c`)  

Contributions are welcome! Pull requests & ideas appreciated  

---

## ☕ Support

If you like this project and want to support development:  SilkQuizzer

[![Buy Me A Coffee](https://img.shields.io/badge/-Buy%20Me%20a%20Coffee-ffdd00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/yourusername)




## ⚙️ How to Use
1. Open project in STM32CubeIDE  
2. Flash to Nucleo-STM32G071RB  
3. Connect ILI9341 TFT SPI (240×320)  
4. 5 pins for display and 4 for BTN in pullUP witchi GND
