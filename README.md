# Ejercicios de Graficacion y Grafos con Raylib

Coleccion de ejercicios en C++17 desarrollados con la biblioteca grafica Raylib. El proyecto incluye tanto ejecutables independientes para cada practica como una aplicacion principal unificada con barra de navegacion para alternar entre las vistas en tiempo real.

## Contenido del proyecto

El repositorio consta de siete ejercicios:

1. **Circulo (`circle.cpp`)**: Renderizado basico 2D de una primitiva circular centrada.
2. **Cara seria (`serious_face.cpp`)**: Composicion de figuras geometricas 2D (circulos y trazos con grosor).
3. **Cubo 2D (`cube_2d.cpp`)**: Proyeccion isometrica de un cubo en 2D definida mediante un arreglo de 8 vertices y 12 aristas.
4. **Cubo 3D (`cube.cpp`)**: Escena tridimensional con camara en perspectiva. En la vista unificada permite rotar la camara arrastrando con el boton izquierdo del raton y restablecer la posicion con la tecla `R`.
5. **Grafo no dirigido (`graph.cpp`)**: Representacion visual del grafo completo $K_4$ (4 vertices y 6 aristas).
6. **Recorrido DFS (`dfs_graph.cpp`)**: Visualizacion interactiva de busqueda en profundidad (Depth-First Search) sobre un arbol jerarquico de 20 nodos (niveles 0 al 4).
7. **Recorrido BFS (`bfs_graph.cpp`)**: Visualizacion interactiva de busqueda en amplitud (Breadth-First Search) sobre el mismo arbol de 20 nodos.

### Estructura de archivos

- `main.cpp`: Punto de entrada de la aplicacion unificada. Gestiona la ventana redimensionable, la camara orbital 3D, el area de recorte y el pie de pagina informativo.
- `navbar.hpp`: Implementacion de la barra superior para cambiar de ejercicio con el raton, las teclas numericas (`1` al `7`) o las flechas de direccion (`Izquierda` / `Derecha`).
- `exercises.hpp`: Funciones de dibujo de cada ejercicio y registro central de vistas.
- `graph_traversal.hpp`: Logica del grafo de 20 nodos, calculo de posiciones en pantalla, algoritmos de sugerencia paso a paso para DFS y BFS, historial del camino recorrido e inspeccion de grado y vecinos del vertice seleccionado.
- `circle.cpp`, `serious_face.cpp`, `cube_2d.cpp`, `cube.cpp`, `graph.cpp`, `dfs_graph.cpp`, `bfs_graph.cpp`: Programas individuales para compilar y probar cada ejercicio por separado.

---

## Requisitos

- Compilador de C++ con soporte para **C++17** (`g++` o `clang++`).
- Biblioteca **Raylib** (version 4.5 o 5.x).

---

## Instalacion de Raylib

### Windows (MSYS2 / MinGW-w64 UCRT64)

Esta es la configuracion recomendada y la que utiliza el proyecto en `.vscode/`.

1. Descarga e instala [MSYS2](https://www.msys2.org/).
2. Abre la terminal **MSYS2 UCRT64** y actualiza la base de paquetes:
   ```bash
   pacman -Syu
   ```
3. Instala la cadena de herramientas de C++ y Raylib para el entorno UCRT64:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-raylib
   ```
4. Agrega la ruta `C:\msys64\ucrt64\bin` a la variable de entorno `PATH` de Windows para poder invocar `g++` desde cualquier terminal.

### Linux

Instala el compilador, las dependencias de OpenGL/X11 y Raylib segun tu distribucion:

**Arch Linux / Manjaro:**
```bash
sudo pacman -S base-devel raylib
```

**Fedora:**
```bash
sudo dnf install gcc-c++ raylib-devel
```

**Ubuntu / Debian:**
En versiones recientes que incluyan el paquete en los repositorios oficiales:
```bash
sudo apt update
sudo apt install build-essential libraylib-dev
```

Si tu version de Ubuntu o Debian no incluye `libraylib-dev`, puedes compilar e instalar Raylib desde el codigo fuente:
```bash
sudo apt update
sudo apt install build-essential git cmake libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev

git clone --depth 1 https://github.com/raysan5/raylib.git
cd raylib
cmake -B build -DBUILD_SHARED_LIBS=ON
cmake --build build
sudo cmake --install build
sudo ldconfig
```

### macOS

Utilizando [Homebrew](https://brew.sh/):

1. Instala las herramientas de linea de comandos de Xcode si aun no las tienes:
   ```bash
   xcode-select --install
   ```
2. Instala Raylib con Homebrew:
   ```bash
   brew install raylib
   ```

---

## Compilacion y ejecucion

Los archivos de cabecera son auto-contenidos (`header-only`), por lo que basta con compilar el archivo `.cpp` deseado (`main.cpp` para la aplicacion completa o cualquiera de los archivos individuales) enlazando contra Raylib.

### Windows (PowerShell o terminal UCRT64)

Para compilar y ejecutar la aplicacion principal:
```powershell
g++ -std=c++17 main.cpp -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm
.\main.exe
```

Para compilar un ejercicio individual (por ejemplo, `dfs_graph.cpp`):
```powershell
g++ -std=c++17 dfs_graph.cpp -o dfs_graph.exe -lraylib -lopengl32 -lgdi32 -lwinmm
.\dfs_graph.exe
```

> Nota: Si abres la carpeta en Visual Studio Code en Windows con MSYS2 instalado en `C:/msys64/ucrt64`, puedes abrir cualquier archivo `.cpp` y presionar `Ctrl+Shift+B` para ejecutar la tarea de compilacion configurada en `.vscode/tasks.json`.

### Linux

Para compilar y ejecutar la aplicacion principal:
```bash
g++ -std=c++17 main.cpp -o main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./main
```

Para compilar un ejercicio individual:
```bash
g++ -std=c++17 bfs_graph.cpp -o bfs_graph -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./bfs_graph
```

### macOS

Usando `pkg-config` (instalado junto con Homebrew o mediante `brew install pkg-config`):
```bash
clang++ -std=c++17 main.cpp -o main $(pkg-config --cflags --libs raylib) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
./main
```

O indicando las rutas de Homebrew de forma explicita:
```bash
clang++ -std=c++17 main.cpp -o main -I$(brew --prefix raylib)/include -L$(brew --prefix raylib)/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
./main
```

---

## Controles en la aplicacion

- **Navegacion entre ejercicios (`main`)**:
  - Clic izquierdo sobre las pestañas de la barra superior.
  - Teclas numericas `1` al `7`.
  - Flechas `Izquierda` y `Derecha` para avanzar o retroceder.
- **Vista Cubo 3D**:
  - Arrastrar con clic izquierdo dentro del lienzo: rotar la camara orbital.
  - Tecla `R`: restablecer la camara a su posicion inicial.
- **Vistas DFS y BFS (Arbol de 20 nodos)**:
  - Clic izquierdo sobre un nodo: marcar el vertice como visitado, actualizar el camino en la barra superior, consultar su grado y vecinos en la barra inferior, y resaltar con flechas verdes el siguiente paso sugerido por el algoritmo.
  - Tecla `R` o clic derecho dentro del lienzo: reiniciar el recorrido.
