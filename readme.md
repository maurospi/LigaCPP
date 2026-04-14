# Fabián Mauricio Ospina Arévalo

---

## Descripcion del programa

Programa de consola en C++ para manejar una liga de futbol. Registra partidos, calcula la tabla de posiciones, muestra el historial de jornadas y permite editar o borrar resultados. Permite editar los clubes desde la terminal. Todo funciona con archivos de texto entre ejecuciones.

---

## Compilar y ejecutar

```bash
g++ -std=c++11 -o liga src/main.cpp
./liga
```

Necesitas `data/config.txt` antes de ejecutar.

---

## Menú del programa

1. Ver tabla de posiciones
2. Registrar partido
3. Ver historial de jornadas
4. Ver todos los partidos
5. Modificar equipos
6. Eliminar partido
7. Editar resultado
8. Salir

---

## Formato de config.txt

### Nombre de la liga
```
league=Liga Ospina
```
### Configuración de puntos
```
win_points=3
draw_points=1
defeat_points=0
```
### Configuracion de clubes
```
club=Real Vardrid
club=Millitos el mas grande
club=Fracalona
club=El JuJu
club=Inter de Bogota
club=Arsenal
club=Al Nassr
club=Inter Miami
club=Deportes Tolima
club=Patriotas
club=Villareal
club=Prueba Agragar fc
```

Campos obligatorios: `league, win_points, draw_points, defeat_points`. Las lineas con `#` son comentarios.

---

## Estructura de archivos

```
src/
    main.cpp
  data/
    config.txt      // configuracion de la liga
    partidos.txt    // un partido por linea
    fechas.txt      // partidos agrupados por jornada
    tabla.txt       // se genera al guardar la tabla
  README.md
  .gitignore
```

---

## Decisiones de diseno

**Dos archivos para los partidos**  
`partidos.txt` tiene un partido por linea para facilitar edicion y eliminacion. `fechas.txt` agrupa los mismos partidos por jornada con bloques `JORNADA=` y `FIN_JORNADA` para mostrar el historial.

**Puntero en updateStats**  
Recibe `Club*` para modificar el equipo original del vector. Con una copia los cambios se perderian al salir de la funcion.

**Puntos configurables**  
Los puntos por resultado se leen de `config.txt` y no estan fijos en el codigo. Si cambian las reglas solo se edita el archivo.

**Edicion y eliminacion de partidos**  
El programa lee el archivo completo en memoria, aplica el cambio en el vector y reescribe el archivo. Aplica para `partidos.txt` y `fechas.txt`.

**Calculo de la tabla**  
Se recalcula desde cero cada vez que el usuario la pide leyendo todos los partidos de `partidos.txt` y se define la clasificacion en base a las posiciones.