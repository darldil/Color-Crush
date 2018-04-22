//Color Crush 2.1.2
//Propiedad de Alvaro Bressel Lopez y Mauricio Abbati Loureiro.
//*******Por favor, antes de comenzar ejecuta el archivo "Léame"********//

/*---------------------NOTAS DE LA VERSION 2.1.2----------------//
La Versión 2.1.2 incorpora la capacidad de guardar partida, guardar
puntuación, cargar tablero personalizado y la opcion modo "gelatina".
IMPORTANTE: El juego incopora características de reproducción de audio,
para utilizarlas es necesario guardar el archivo wav "System_Music" en
la ruta de la aplicación. El archivo no es necesario para ejecutar la 
aplicación.
*/
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <string>
using namespace std;
#include <iomanip>
#include <fstream>
#include <direct.h> //Se usa para crear carpetas.
#include <Windows.h> //Se utiliza para los Sleep(), Beep() y para PlaySound()
#pragma comment (lib, "winmm.lib") //Vinculador, se utiliza para reproducir música en formato WAV


/*----------------CONSTANTES----------------------*/
const bool DEBUG = false; //Modo desarrollador, con pausas durante la partida (false = desactivado).
const bool GELATINA = true; //Modo predeterminado del modo "Gelatina" (false = desactivado).
const int FILAS = 8;
const int COLUMNAS = 8;
const int COLORES = 4;
const int INTENTOS = 15;
const int MINIMO_GRUPOS = 3; //Determina el minimo de fichas para considerarlas como grupo. Valor recomendado 3 mínimo.
const int SEPARACIONCELDAS = 20; //Establece el centrado en pantalla de los setw(). Valor recomendado = 20
const int TIEMPO = 500; //Determina el tiempo de pausa de los Sleep().
const string VERSION = "Color Crush 2.1.2"; //Indica la versión actual
/*--------------------------------------------------------------------------------*/

/*-------------------------------------ARRAYS Y ENUMERADOS--------------------------------------------------*/
typedef enum {Vacio, Magenta, Amarillo, Azul, Verde} tColorFicha; //Colores de las fichas con las que vamos a trabajar.
typedef enum {Iz, Der, Sub, Baj} tDireccion; //Indica las direcciones que puede elegir el usuario.
typedef enum { 
	black, // 0
	dark_blue, // 1
	dark_green, // 2
	dark_cyan, // 3
	dark_red, // 4
	dark_magenta, // 5
	dark_yellow, // 6
	light_gray, // 7
	dark_gray, // 8
	light_blue, // 9
	light_green, // 10
	light_cyan, // 11
	light_red, // 12
	light_magenta, // 13
	light_yellow, // 14
	white // 15
} tColorScreen; //Colores que muestra Windows por pantalla.
typedef bool tGrupos[FILAS][COLUMNAS]; //Array de booleanos para verificar grupos y para la gelatina.
typedef tColorFicha tTablero[FILAS][COLUMNAS]; //Tablero para cargar los enumerados.
typedef struct {
	tTablero tablero;
	tGrupos gelatina;
	int intentos;
	int puntuacion;
}tJuego; //Estructura general del juego.
/*---------------------------------------------------------------------------------------------------------------*/


/*----------------------------------DECLARACION DE FUNCIONES-------------------------------------------------- */
string pedirNombre(); //Solicita el nombre de usuario al iniciarse el juego.
char menu(bool music, bool gelatina); //Lee la opción que introduce el usuario (se usa char para evitar problemas con caracteres). 
bool opcionGelatina(bool gelatina);
/*
	Permite activar o desactivar el modo "gelatina" al usuario.
	ENTRADA: booleando gelatina, que indica si el modo gelatina está activado.
	SALIDA: booleano que activar o desactica dicho modo.
*/
bool reproducirMusica(bool musica); 
/*
	Permite al usuario decidir si se reproduce o no la musica.
	ENTRADA: booleano musica, que indica si la música se está reproduciendo.
	SALIDA: booleano que inicia o detiene la reproducción.
*/
tColorFicha claveAleatoria(); 
/*
	Generador de fichas aleatorias.
	ENTRADA: Nada.
	SALIDA: Un único enumerado.
*/
bool cargarTablero(tJuego &juego, string fichero); 
/*
	Carga un tablero desde un fichero del usuario.
	ENTRADA: Estructura juego y el nombre del archivo a cargar.
	SALIDA: Booleano que indica si la carga se ha realizado con éxito.
*/
void crearTablero(tJuego &juego, bool &gel, bool crearGel); 
/*
	Crea un tablero completamente aleatorio.
	ENTRADA: Estructura del juego, booleano que indica si existe gelatina en el tablero y booleano que indica si se debe generar
	gelatina.
	SALIDA: Tablero creado y booleano indicador de gelatina.
*/
void mostrarTablero(const tJuego juego, bool gelatina); 
/*
	Ejecuta las instancias necesarias para mostrar el tablero por pantalla.
	ENTRADA: Estructura del juego y booleano indicador de gelatina.
	SALIDA: Internamente no devuelve ningún dato, externamente muestra la información del tablero por pantalla.
*/
void crearPrimeraFila(); 
/*	
	Genera la primera linea del tablero.
	ENTRADA: Nada.
	SALIDA: Muestra por pantalla la primera linea del tablero.
*/
void componerFilas(const tJuego juego, const tColorScreen color); 
/*
	Crea las casillas intermedias del tablero.
	ENTRADA: Estructura del juego y color predeterminado para mostrar en pantalla.
	SALIDA: Casillas del tablero en pantalla con sus correspondientes fichas.
*/
void crearUltimaFila(); 
/*
	Genera la ultima linea del tablero.
	ENTRADA: Nada.
	SALIDA: Muestra por pantalla la última linea del tablero.
*/
void mostrarFicha(const tJuego juego, const tColorScreen col, int f, int c); 
/*
	Muestra las fichas dentro del tablero tablero.
	ENTRADA: Estructura del juego, color predeterminado para mostrar en pantalla, filas y columnas actuales.
	SALIDA: Muestra en pantalla la ficha.
*/
void establecerColor(tColorScreen color); 
/*
	Cambia el color de un caracter dado.
	ENTRADA: Color elegido.
	SALIDA: Configuración para mostrar en pantalla dicho color.
*/
void detectarGruposFilas(tTablero tablero, tGrupos busca, tGrupos gel, int &puntos, bool &seguir); 
/*
	Busca grupos de colores colores por filas, el numero de grupos está establecido por MINIMO_GRUPOS.
	ENTRADA: Tablero de juego, array de booleanos que determina los grupos, array de booleanos para la gelatina, puntos y booleano que 
	indica si se debe realizar una nueva vuelta.
	SALIDA: Array de booleanos con grupos ya detectados, array de gelatina con elementos ya anulados (si se diese el caso), suma de puntos 
	por la jugada y booleano que indica si se debe realizar una nueva vuelta.
*/
void detectarUnaFila(tTablero tablero, tGrupos busca, tGrupos gel, int f, bool &seguir);
/*
	Busca grupos de colores colores en una fila determinada, el numero de grupos está establecido por MINIMO_GRUPOS.
	ENTRADA: Tablero de juego, array de booleanos que determina los grupos, array de booleanos para la gelatina, fila y booleano que 
	indica si se debe realizar una nueva vuelta.
	SALIDA: Array de booleanos con grupos ya detectados, array de gelatina con elementos ya anulados (si se diese el caso) 
	y booleano que indica si se debe realizar una nueva vuelta.
*/
void detectarGruposColumnas(tTablero tablero, tGrupos busca, tGrupos gel, int &puntos, bool &seguir); 
/*
	Busca grupos de colores colores por columnas, el numero de grupos está establecido por MINIMO_GRUPOS.
	ENTRADA: Tablero de juego, array de booleanos que determina los grupos, array de booleanos para la gelatina, puntos y booleano que 
	indica si se debe realizar una nueva vuelta.
	SALIDA: Array de booleanos con grupos ya detectados, array de gelatina con elementos ya anulados (si se diese el caso), suma de puntos 
	por la jugada y booleano que indica si se debe realizar una nueva vuelta.
*/
void detectarUnaColumna(tTablero tablero, tGrupos busca, tGrupos gel, int c, bool &seguir);
/*
	Busca grupos de colores colores en una columna determinada, el numero de grupos está establecido por MINIMO_GRUPOS.
	ENTRADA: Tablero de juego, array de booleanos que determina los grupos, array de booleanos para la gelatina, columna y booleano que 
	indica si se debe realizar una nueva vuelta.
	SALIDA: Array de booleanos con grupos ya detectados, array de gelatina con elementos ya anulados (si se diese el caso), y
	booleano que indica si se debe realizar una nueva vuelta.
*/
void procesarTablero(tJuego &juego, bool &gel); 
/*
	Hace de intermediario para  verificar el tablero, borrar los grupos y crear nuevas fichas
	ENTRADA: Estrcutura del juego y booleano que indica si hay gelatina.
	SALIDA: Estructura del juego con las modificaciones pertinentes realizadas y booleano que indica si existe gelatina en el tablero.
*/
bool bajarFichas(tTablero tablero); 
/*
	Baja las fichas a los espacios vacíos y genera una nueva ficha aleatoria.
	ENTRADA: Tablero del juego.
	SALIDA: Tablero del juego modificado.
*/
bool opcionUsuario(const tJuego juego, int &f, int &c, tDireccion &mover); 
/*
	Permite seleccionar al usuario su jugada.
	ENTRADA: Estructura del juego.
	SALIDA: Booleano que indique si se desea continuar con la partida, fila y columna elegida y dirección a mover la ficha.
*/
bool verificarOpcionUsuario(int fila, int columna, char movimiento, tDireccion &mover); 
/*
	Comprueba que la información insertada por el usuario sea válida.
	ENTRADA: Fila elegida, columna elegida, dirección a mover.
	SALIDA: Enumerado de dirección de movimiento.
*/
bool verificarMovimiento(tDireccion mover, const tJuego juego, int f, int c); 
/*
	Comprueba si el movimiento del usuario genera un grupo.
	ENTRADA: Dirección a mover, estructura del juego, fila elegida y columna.
	SALIDA: Booleano de verificación.
*/
void moverTablero(tTablero tablero, tDireccion mover, int f, int c); 
/*
	Mueve las fichas en función de la jugada introducida.
	ENTRADA: Dirección escogida, fila escogida y columna escogida.
	SALIDA: Tablero modificado.
*/
int puntuacion(int numeroFichas, int puntosAc); 
/* 
	Calcula la puntuación de cada jugada.
	ENTRADA: Grupo detectado y puntuación acumulada.
	SALIDA: Puntuación actual.
*/
void cargarPartida(tJuego &juego, bool gelatina); 
/*
	Carga una partida guardada en el archivo "SaveData.dat".
	ENTRADA: Booleano de gelatina.
	SALIDA: Partida (Estructura del juego) cargada. 
*/
void guardarPartida(const tJuego juego, bool gelatina, bool hayGelatina); 
/*
	Guarda una partida en el archivo "SaveData.dat"
	ENTRADA: Partida (Estructura del juego), booleano de gelatina y booleano de verificación sobre la existencia de gelatina
	en el tablero.
	SALIDA: Nada.
*/
void mostrarPuntuacion(); //Muestra la puntuación del usuario almacenada en el archivo "Puntuacion.dat"
void guardarPuntuacionTemporal(string nombre, int puntuacion); 
/*
	Guarda las puntuaciones en un archivo temporal.
	ENTRADA: Nombre de usuario y puntuación.
	SALIDA: Nada.
*/
void guardarPuntuacion(string nombre, bool encontrado, bool abierto, int puntuacion); 
/*
	Guarda las puntuaciones en el archivo "Puntuacion.dat"
	ENTRADA: Nombre de usuario, booleano que verifica si se han encontrado jugadores anteriores, booleano que comprueba si tmp.txt
	está abierto y puntuación.
	SALIDA: Nada.
*/
void colorCrush(string nombre, string fichero, bool carga, bool gelatina); 
/*
	Juego principal
	ENTRADA: Nombre de usuario, nombre del fichero personal del usuario, booleano que verifica si se debe cargar de plantilla
	y booleano si debe existir gelatina.
*/

/*---------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------INICIO DEL JUEGO--------------------------------------------------- */
int main() {

	srand(time(NULL));
	establecerColor(white);
	char opcionMenu;
	bool cargar = false, musica = true, gelatina = GELATINA;
	string nombre, fichero;

	_mkdir("ColorCrushData");
	PlaySound("System_Music.wav", NULL, SND_NODEFAULT | SND_LOOP | SND_FILENAME | SND_ASYNC | SND_NOSTOP | SND_SYSTEM);
	/*
		Reproduce la música principal del juego indefinidamente, sin reproducir mensaje de error en caso de no existir
		el archivo y con control de volumen adaptado a Windows Vista o superior.
		El archivo de música NO ES OBLIGATORIO para que funcione el programa.
	*/

	cout << setw(49) << VERSION << endl;

	cout << endl << "Bienvenido a Color Crush." << endl;
	nombre = pedirNombre();
	cout << "Bienvenido(a) " << nombre << endl;

	if (DEBUG) {
		cout << endl << "Precaucion! El modo debug esta activado." << endl;
		mciSendString ("play C:\\Windows\\Media\\notify.wav", NULL, 0, NULL); /*A parte de la música se utilizan sonidos
		del sistema por toda la interfaz del programa. */
		Sleep(TIEMPO);
	}

	opcionMenu = menu(musica, gelatina); 

	while (opcionMenu != '0') {
		switch (opcionMenu)
		{
		case '1':
			{
				colorCrush(nombre, fichero, cargar, gelatina);
				
				cargar = false; //Desactiva la carga de tablero personalizado en caso de estar activado.
				cout << endl << "Fin de la partida." << endl;
				Sleep(TIEMPO + 500);
				system("cls"); //Limpia la pantalla.
			}
			break;

		case '2':
			{
				gelatina = opcionGelatina(gelatina);
			}
			break;

		case '3':
			{
				mostrarPuntuacion();

				if (!DEBUG) {
					Sleep(TIEMPO + 1000);
				}
				else {
					system("pause");
				}
				cout << endl;
			}
			break;

		case '4':
			{
				cout << endl << "Por favor, indique el nombre del tablero: ";
				cin >> fichero;

				if (nombre != "0") {
					cargar = true; //Activa la carga del tablero personalizado.
					cout << "De acuerdo. Se procedera a cargar el tablero cuando pulse jugar." << endl << endl;
				}
			}
			break;

		case '5':
			{
				musica = reproducirMusica(musica);
			}
			break;
		}
		cout << setw(49) << VERSION << endl;
		opcionMenu = menu(musica, gelatina);
	}

	if(!DEBUG) {
		PlaySound(NULL, 0, 0); //Desactiva la música principal.
		cout << endl << setw(56) << "Gracias por jugar! Hasta pronto" << endl;
		Sleep(TIEMPO * 2);
		PlaySound ("C:\\Windows\\Media\\tada.wav", NULL, SND_NODEFAULT | SND_FILENAME);
	}

	return 0;
}


string pedirNombre() {

	string nombre;

	cout << "Para comenzar, por favor introduza su nombre: ";
	cin >> nombre;
	cin.sync();

	return nombre;
}


char menu(bool music, bool gelatina) {

	char opcion;

	cout << endl;
	cout << "Menu Principal." << endl;
	cout << "1 - Jugar" << endl;

	if (gelatina) {
		cout << "2 - Desactivar Modo 'Gelatina'" << endl;
	}
	else {
		cout << "2 - Activar Modo 'Gelatina'" << endl;
	}

	cout << "3 - Mostrar Puntuacion" << endl;
	cout << "4 - Cargar Plantilla Tablero" << endl;

	if (music) {
		cout << "5 - Detener Musica" << endl;
	}
	else {
		cout << "5 - Reproducir Musica" << endl;
	}

	cout << "0 - Salir" << endl;

	cout << "Seleccione una opcion: ";
	cin >> opcion;
	cin.sync();

	while (opcion < '0' || opcion > '5') {
		cout << "Opcion Incorrecta, introduzcala de nuevo: ";
		cin >> opcion;
		cin.sync();
	}
	return opcion;
}


bool opcionGelatina(bool gelatina) {

	bool resultado;

	if (gelatina) {
		resultado = false;
	}
	
	else {
		resultado = true;
	}
	
	system("cls");

	return resultado;
}


bool reproducirMusica(bool musica) {

	bool resultado;

	if (musica) {
		PlaySound(NULL, 0, 0);
		resultado = false;
	}

	else {
		PlaySound("System_Music.wav", NULL, SND_NODEFAULT | SND_LOOP | SND_FILENAME | SND_ASYNC | SND_NOSTOP | SND_SYSTEM);
		resultado = true;
	}
	system("cls");

	return resultado;
}


tColorFicha claveAleatoria() {

	tColorFicha colorSecreto;

	colorSecreto = tColorFicha(rand() % COLORES + 1); //Genera el enumerado aleatorio.

	return colorSecreto;
}


bool cargarTablero(tJuego &juego, string fichero) {

	bool cargado = false;
	ifstream entrada;
	int aux;

	entrada.open(fichero);

	if (!entrada.is_open()) {
		cout << endl << "ERROR! Ha ocurrido un problema al cargar su tablero." << endl;
		cout << "Se procede a crear un tablero nuevo..." << endl;
		Sleep(TIEMPO + 1000);
	}

	else {

		cout << endl << "Se ha encontrado el archivo de tablero." << endl;
		cout << "Se procede a realizar la carga..." << endl;
		Sleep(TIEMPO + 1000);

		juego.intentos = INTENTOS;
		juego.puntuacion = 0;

		for (int f = 0; f < FILAS; f++) { //Este 'for' carga las fichas en el tablero.
			for (int c = 0; c < COLUMNAS; c++) {
				entrada >> aux;
				juego.tablero[f][c] = tColorFicha(aux);
			}
		}

		for (int f = 0; f < FILAS; f++) { //Este 'for'carga la gelatina en el tablero.
			for (int c = 0; c < COLUMNAS; c++) {
				entrada >> juego.gelatina[f][c];
			}
		}
		cargado = true; //Indica que se ha cargado correctamente el tablero en la partida.
	}
	return cargado;
}


void crearTablero(tJuego &juego, bool &gel, bool crearGel) {

	juego.intentos = INTENTOS;
	juego.puntuacion = 0;

	for (int f = FILAS - 1; f >= 0; f--) { //Este 'for' genera las fichas.
		for (int c = 0; c < COLUMNAS; c++) {
			juego.tablero[f][c] = claveAleatoria();
		}
	}

	if (crearGel) { //Si el modo gelatina está activado este 'for' genera la gelatina.
		for (int f = FILAS - 1; f >= 0; f--) {
			for (int c = 0; c < COLUMNAS; c++) {
				juego.gelatina[f][c] = rand() % 2;
				gel = true;
			}
		}
	}

	else {
		for (int f = FILAS - 1; f >= 0; f--) {
			for (int c = 0; c < COLUMNAS; c++) {
				juego.gelatina[f][c] = false;
			}
		}
	}
}


void mostrarTablero(const tJuego juego, bool gelatina) {

	tColorScreen color = white;
	int cuentaFilas = 0, numeroF = FILAS;

	if (!DEBUG) {
		system("cls");
		cout << setw(49) << VERSION << endl;
	}

	cout << endl;
	if (gelatina){
		cout << setw(SEPARACIONCELDAS + 12) << "Intentos: " << juego.intentos << setw(13) << " Gelatina" << setw(13) << "Puntos: " << juego.puntuacion;
	}

	else {
		cout << setw(SEPARACIONCELDAS + 17) << "Intentos: " << juego.intentos << setw(13) << "Puntos: " << juego.puntuacion;
	}

	establecerColor(color);

	crearPrimeraFila();
	componerFilas(juego, color);
	crearUltimaFila();

	for (int n = 1; n < COLUMNAS + 1; n++) {
		cout << n << setw(4);
	}

	cout << endl << endl;

	if (DEBUG) {
		system("pause");
	}

	else {
		Sleep(TIEMPO);
	}
}


void crearPrimeraFila() {

	cout << endl << setw(SEPARACIONCELDAS + 5) << char(218);

	for (int c = 0; c < COLUMNAS - 1; c++) {
		cout << char(196) << char(196) << char(196) << char(194);
	}

	cout << char(196) << char(196) << char(196);
	cout << char(191) << setw(SEPARACIONCELDAS + 5) << endl;
}


void componerFilas(const tJuego juego, const tColorScreen color) {

	int filas = FILAS - 1, numFilas = FILAS;

	while (filas >= 0) {
		cout << setw(SEPARACIONCELDAS + 4) << numFilas;
		for (int c = 0; c < COLUMNAS + 1; c++) {
			cout << char(179);
			if (c < COLUMNAS) {
				mostrarFicha(juego, color, filas, c);
			}
		}
		cout << setw(SEPARACIONCELDAS + 5) << endl;

		if (filas != 0) {

			cout << char(195);

			for (int c = 0; c < COLUMNAS - 1; c++) {
				cout << char(196) << char(196) << char(196)	<< char(197);
			}
			cout << char(196) << char(196) << char(196);
			cout << char(180) << endl << setw(SEPARACIONCELDAS + 4);
		}
		filas--;
		numFilas--;
	}
}


void crearUltimaFila() {

			cout << char(192);

			for (int c = 0; c < COLUMNAS - 1; c++) {
				cout << char(196) << char(196) << char(196) << char(193);
			}
			cout << char(196) << char(196) << char(196);
			cout << char(217) << endl << setw(SEPARACIONCELDAS + 7);
}


void mostrarFicha(const tJuego juego, const tColorScreen col, int f, int c) {

	int vueltas = 3;

	if (juego.gelatina[f][c]) { //Si hay gelatina esto muestra por pantalla una ficha blanca.
		establecerColor(white);
		cout << char(219);
		vueltas = 1;
	}

	for (int n = 0; n < vueltas; n++) {
		switch(juego.tablero[f][c]) {
		case Vacio:	establecerColor(black);	break;
		case Magenta: establecerColor(light_red); 	break;
		case Amarillo:	establecerColor(light_yellow); 	break;
		case Azul:	establecerColor(light_blue);	break;
		case Verde:	establecerColor(light_green);	break;
		}
		cout << char(219);
	}
	if (juego.gelatina[f][c]) {
		establecerColor(white);
		cout << char(219);
	}

	establecerColor(col);
}


void establecerColor(tColorScreen color) {

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(handle, color);
}


void detectarGruposFilas(tTablero tablero, tGrupos gel, tGrupos busca, int &puntos, bool &seguir) {

	tColorFicha previousColor = Vacio, currentColor;
	int contador = 0, cuentaPuntos = 0;
	seguir = false;

	for (int f = 0; f < FILAS; f++) { //Aquí se detectan los grupos por filas y se aplica la suma de puntos.
		for (int c = 0; c < COLUMNAS + 1; c++) {
			currentColor = tablero[f][c];

			if (previousColor == currentColor && previousColor != Vacio) {
				contador++; 
			}

			if (previousColor != currentColor && contador < (MINIMO_GRUPOS - 1)) {
				contador = 0;
			}

			if (previousColor != currentColor && contador >= (MINIMO_GRUPOS - 1)) {
				for (int cont = contador; cont >= 0; cont--) {
					busca[f][c - (cont + 1)] = true;
					gel[f][c - (cont + 1)] = false;
					cuentaPuntos++;
					seguir = true;
				}
				contador = 0;
				puntos = puntuacion(cuentaPuntos, puntos);
				cuentaPuntos = 0;
			}
			previousColor = currentColor;
		}
		if (contador >= (MINIMO_GRUPOS)) {
			for (int cont = contador; cont > 0; cont--) {
				busca[f][(COLUMNAS - 1) - (cont - 1)] = true;
				gel[f][(COLUMNAS - 1) - (cont - 1)] = false;
				cuentaPuntos++;
				seguir = true;
			}
			puntos = puntuacion(cuentaPuntos, puntos);
		}
		previousColor = Vacio;
		contador = 0;
	}
}


void detectarUnaFila(tTablero tablero, tGrupos busca, tGrupos gel, int f, bool &seguir) {

	tColorFicha previousColor = Vacio, currentColor;
	int contador = 0;
	seguir = false;

	for (int c = 0; c < COLUMNAS + 1; c++) {
		currentColor = tablero[f][c];

		if (previousColor == currentColor && previousColor != Vacio) {
			contador++; 
		}

		if (previousColor != currentColor && contador < (MINIMO_GRUPOS - 1)) {
			contador = 0;
		}

		if (previousColor != currentColor && contador >= (MINIMO_GRUPOS - 1)) {
			for (int cont = contador; cont >= 0; cont--) {
				busca[f][c - (cont + 1)] = true;
				gel[f][c - (cont + 1)] = false;
				seguir = true;
			}
			contador = 0;
		}
		previousColor = currentColor;
	}

	if (contador >= (MINIMO_GRUPOS)) {
		for (int cont = contador; cont > 0; cont--) {
			busca[f][(COLUMNAS - 1) - (cont - 1)] = true;
			gel[f][(COLUMNAS - 1) - (cont - 1)] = false;
			seguir = true;
		}
	}
}


void detectarGruposColumnas(tTablero tablero, tGrupos gel, tGrupos busca, int &puntos, bool &seguir) {

	tColorFicha previousColor = Vacio, currentColor;
	int contador = 0, cuentaPuntos = 0;

	for (int c = 0; c < COLUMNAS; c++) { //Aquí se detectan los grupos por columnas y se aplica la suma de puntos.
		for (int f = 0; f < FILAS + 1; f++) {
			currentColor = tablero[f][c];

			if (previousColor == currentColor && previousColor != Vacio) {
				contador++; 
			}

			if (previousColor != currentColor && contador < (MINIMO_GRUPOS - 1)) {
				contador = 0;
			}

			if (previousColor != currentColor && contador >= (MINIMO_GRUPOS - 1)) {
				for (int cont = contador; cont >= 0; cont--) {
					busca[f - (cont + 1)][c] = true;
					gel[f - (cont + 1)][c] = false;
					cuentaPuntos++;
					seguir = true;
				}
				contador = 0;
				puntos = puntuacion(cuentaPuntos, puntos);
				cuentaPuntos = 0;
			}
			previousColor = currentColor;
		}
		if (contador >= (MINIMO_GRUPOS - 1)) {
			for (int cont = contador; cont >= 0; cont--) {
				busca[(FILAS - 1) - (cont + 1)][c] = true;
				gel[(FILAS - 1) - (cont + 1)][c] = false;
				cuentaPuntos++;
				seguir = true;
			}
			puntos = puntuacion(cuentaPuntos, puntos);
		}
		previousColor = Vacio;
		contador = 0;
	}
}


void detectarUnaColumna(tTablero tablero, tGrupos busca, tGrupos gel, int c, bool &seguir) {

	tColorFicha previousColor = Vacio, currentColor;
	int contador = 0;

	for (int f = 0; f < FILAS + 1; f++) {
		currentColor = tablero[f][c];

		if (previousColor == currentColor && previousColor != Vacio) {
			contador++; 
		}

		if (previousColor != currentColor && contador < (MINIMO_GRUPOS - 1)) {
			contador = 0;
		}

		if (previousColor != currentColor && contador >= (MINIMO_GRUPOS - 1)) {
			for (int cont = contador; cont >= 0; cont--) {
				busca[f - (cont + 1)][c] = true;
				gel[f - (cont + 1)][c] = false;
				seguir = true;
			}
			contador = 0;
		}
		previousColor = currentColor;
	}
		
	if (contador >= (MINIMO_GRUPOS - 1)) {
		for (int cont = contador; cont >= 0; cont--) {
			busca[(FILAS - 1) - (cont + 1)][c] = true;
			gel[(FILAS - 1) - (cont + 1)][c] = false;
			seguir = true;
		}
	}
}


void procesarTablero(tJuego &juego, bool &gel) {

	tGrupos buscador;
	bool desplazadas = false, continuar = true, encontrado = false;

	while (continuar) { //Siempre que haya se puedan eliminar grupos (continuar) se procesará el tablero.

		for (int f = 0; f < FILAS; f++) {
			for (int c = 0; c < COLUMNAS; c++) {
				buscador[f][c] = false;
			}
		}

		detectarGruposFilas(juego.tablero, juego.gelatina, buscador, juego.puntuacion, continuar);
		detectarGruposColumnas(juego.tablero, juego.gelatina, buscador, juego.puntuacion, continuar);

		for (int f = 0; f < FILAS; f++) {
			for (int c = 0; c < COLUMNAS; c++) {
				if (buscador[f][c]) {
					juego.tablero[f][c] = Vacio;
				}
				if (!juego.gelatina[f][c] && !encontrado) {
					gel = false;
				}
				else {
					gel = true;
					encontrado = true;
				}
			}
		}
		
		if (!desplazadas && continuar) {
			mostrarTablero(juego, gel);
		}

		desplazadas = bajarFichas(juego.tablero);

		while (desplazadas) {
			mostrarTablero(juego, gel);
			desplazadas = bajarFichas(juego.tablero);
		}
	}
}


bool bajarFichas(tTablero tablero) {

	bool mover = false, desplazar = false;

	for (int c = 0; c < COLUMNAS; c++) {
		for (int f = 0; f < FILAS; f++) {
			if (tablero[f][c] == Vacio) { //Si se detecta una posición "Vacía" se procederá a mover las fichas.
				if (f != FILAS - 1) {
					tablero[f][c] = tablero[f + 1][c];
				}
				mover = true;
				desplazar = true;
			}
			if (mover) {
				tablero[f][c] = tablero[f + 1][c];
			}
		}

		if (mover) { //Aquí se generan las nuevas fichas.
			tablero[FILAS - 1][c] = claveAleatoria();
		}
		mover = false;
	}

	return desplazar;
}


bool opcionUsuario(const tJuego juego, int &f, int &c, tDireccion &mover) {

	char eleccion;
	bool salir = false, aceptado = false, movimientoValido;

	do {
		cout << "Escriba 0 para salir" << endl;
		cout << "Indique (con espacios) la fila, la columna y el movimiento (D, I, S, B)" << endl;
		cout << "Que desea hacer: ";
		cin >> f;
		f--;	
	
		if (f == -1) {
			salir = true;
		}

		else {
			cin >> c >> eleccion;
			cin.sync();
			c--;
		
			aceptado = verificarOpcionUsuario(f, c, eleccion, mover); //Se comprueba que los datos introducidos sean válidos.
			if (aceptado) {
				movimientoValido = verificarMovimiento(mover, juego, f, c);
				//Se comprueba si con la jugada seleccionada se pueden eliminar grupos.

				if (!movimientoValido) {
					mciSendString ("play C:\\Windows\\Media\\chord.wav", NULL, 0, NULL);
					cout << "No se admite su movimiento. Intentelo de nuevo" << endl;
					aceptado = false;
				}
			}
		}
	}
	while (!aceptado && !salir);

	return salir;
}


bool verificarOpcionUsuario(int fila, int columna, char movimiento, tDireccion &mover) {

	bool valido = false;

	if (fila >= 0 || fila <= (FILAS - 1)) {
		valido = true;
	}

	if (columna >= 0 || columna <= (COLUMNAS - 1)) {
		valido = true;
	}

	movimiento = tolower(movimiento);

	switch(movimiento) {
		case 's': mover = Sub; break;
		case 'b': mover = Baj; break;
		case 'i': mover = Iz; break;
		case 'd': mover = Der; break;
		default: valido = false; break;
	}
	return valido;
}


bool verificarMovimiento(tDireccion mover, const tJuego juego, int f, int c) {

	tTablero tableroAux;
	tGrupos buscarAux, gel;
	bool valido = false, encontrado = false;

	for (int fil = 0; fil < FILAS; fil++) {
		for (int col = 0; col < COLUMNAS; col++) {
			tableroAux[fil][col] = juego.tablero[fil][col];
			gel[fil][col] = juego.gelatina[fil][col];
			buscarAux[fil][col] = false;
		}
	}

	moverTablero(tableroAux, mover, f, c);

	switch(mover) {
	case Sub: f++; break;
	case Baj: f--; break;
	case Der: c++; break;
	case Iz: c--; break;
	}
	
	detectarUnaFila(tableroAux, buscarAux, gel, f, encontrado);
	detectarUnaColumna(tableroAux, buscarAux, gel, c, encontrado);
	
	if (encontrado) {
		valido = true;
	}

	return valido;
}


void moverTablero(tTablero tablero, tDireccion mover, int f, int c) {

	tColorFicha aux;

	switch(mover) {
	case Iz: {
			aux = tablero[f][c];
			tablero[f][c] = tablero[f][c - 1];
			tablero[f][c - 1] = aux;
		}
		break;

	case Der: {
			aux = tablero[f][c];
			tablero[f][c] = tablero[f][c + 1];
			tablero[f][c + 1] = aux;
		}
		break;

	case Sub: {
			aux = tablero[f][c];
			tablero[f][c] = tablero[f + 1][c];
			tablero[f + 1][c] = aux;
		}
		break;

	case Baj: {
			aux = tablero[f][c];
			tablero[f][c] = tablero[f - 1][c];
			tablero[f - 1][c] = aux;
		}
		break;
	}
}


int puntuacion(int numeroFichas, int puntosAc) {

	int puntos = puntosAc;

	if (numeroFichas < (MINIMO_GRUPOS - 1)) {
		puntos = puntosAc;
	}
	
	else if (numeroFichas == MINIMO_GRUPOS)  {
		puntos = puntos + 3;
	}

	else if (numeroFichas == (MINIMO_GRUPOS + 1)) {
		puntos = puntos + 8;
	}

	else {
		puntos = puntos + 15;
	}

	return puntos;
}


void cargarPartida(tJuego &juego, bool gelatina) {

	ifstream entrada;
	string aux;
	int enumAux;

	if (!gelatina) {
		entrada.open("ColorCrushData\\SaveDataNormal.dat");
	}

	else {
		entrada.open("ColorCrushData\\SaveDataJelly.dat");
	}

	if (entrada.is_open()) {
		cout << endl << "Se ha encontrado una partida guardada." << endl;
		cout << "Se procede a realizar la carga..." << endl;

		Sleep(TIEMPO + 1000);

		getline(entrada, aux); //Aquí se ignora el encabezado del archivo de entrada.
		entrada >> juego.intentos;
		entrada >> juego.puntuacion;

		for (int f = 0; f < FILAS; f++) {
			for (int c = 0; c < COLUMNAS; c++) {
				entrada >> enumAux;
				juego.tablero[f][c] = tColorFicha(enumAux);
			}
		}
		
		for (int f = 0; f < FILAS; f++) {
			for (int c = 0; c < COLUMNAS; c++) {
				entrada >> juego.gelatina[f][c];
			}
		}
		entrada.close();
	}

}


void guardarPartida(const tJuego juego, bool gelatina, bool hayGelatina) {

	ofstream salida;

	if (juego.intentos <= 0 && !gelatina) {
		remove("ColorCrushData\\SaveDataNormal.dat");
	}

	else if (gelatina && !hayGelatina || juego.intentos <= 0) {
		remove("ColorCrushData\\SaveDataJelly.dat");
	}

	else {
		if (!gelatina) {
			salida.open("ColorCrushData\\SaveDataNormal.dat");
		}

		else {
			salida.open("ColorCrushData\\SaveDataJelly.dat");
		}

		salida << "Archivo de partida de Color Crush. NO MODIFICAR EXTERNAMENTE." << endl; //Aquí se agrega un encabezado en el archivo de salida.

		salida << juego.intentos << " ";
		salida << juego.puntuacion << endl;

		for (int f = 0; f < FILAS; f++) {
			for (int c = 0; c < COLUMNAS; c++) {
				salida << juego.tablero[f][c] << " ";
			}
			salida << endl;
		}

		for (int f = 0; f < FILAS; f++) {
			for (int c = 0; c < COLUMNAS; c++) {
				salida << juego.gelatina[f][c] << " ";
			}
			salida << endl;
		}
		salida.close();
	}
}


void mostrarPuntuacion() {

	int puntuacion;
	string jugador;
	ifstream carga;

	carga.open("ColorCrushData\\Puntuacion.dat");

	if (!carga.is_open()) {

		cout << endl;
		cout << "ERROR! No existe archivo de puntuaciones." << endl;
	}

	else {

		getline(carga, jugador); //Aquí se ignora el encabezado del archivo de entrada.
		carga >> jugador;

		while (jugador != "xxx") {
			carga  >> puntuacion;

			cout << endl;
			cout << jugador << " " << " " << puntuacion;

			carga >> jugador;
		}
		carga.close();
	}
}


void guardarPuntuacionTemporal(string nombre, int puntuacion) {

	ifstream entradaDatos;
	ofstream salidaDatos;
	string lecturaNombre;
	bool nombreEncontrado = false, abierto = false;
	int puntuacionPartidas = 0;
	int auxPuntos; //Si el usuario existe en usuario.txt carga sus datos en estas variables para actualizarlos posteriormente.

	entradaDatos.open("ColorCrushData\\Puntuacion.dat");
	salidaDatos.open("ColorCrushData\\tmp.txt");

	if(entradaDatos.is_open()) {

		abierto = true;
		getline(entradaDatos, lecturaNombre);
		entradaDatos >> lecturaNombre;

		while (lecturaNombre != "xxx") {

			if (lecturaNombre == nombre) {

				nombreEncontrado = true;
				entradaDatos >> auxPuntos;
				puntuacionPartidas = auxPuntos + puntuacion;
				salidaDatos << " " << lecturaNombre << " " << puntuacionPartidas << endl;
			}

			else { 

				entradaDatos >> puntuacionPartidas;
				salidaDatos << lecturaNombre  << " " << puntuacionPartidas << endl;
			}
			entradaDatos >> lecturaNombre;
		}
		salidaDatos << "xxx";
	}

	salidaDatos.close();
	entradaDatos.close();

	guardarPuntuacion(nombre, nombreEncontrado, abierto, puntuacion);
}


void guardarPuntuacion(string nombre, bool encontrado, bool abierto, int puntuacion) {

	ifstream entradaDatos;
	ofstream salidaDatos;
	string lecturaNombre;
	int puntuacionPartidas = 0;

	entradaDatos.open("ColorCrushData\\tmp.txt");
	salidaDatos.open("ColorCrushData\\Puntuacion.dat");

	if (abierto) {

		if(!entradaDatos.is_open()) {
			cout << "ERROR DE SISTEMA! Por favor, reinicie la aplicacion." << endl;
		}

		else {
			
			salidaDatos << "Archivo de puntuaciones de Color Crush. NO MODIFICAR EXTERNAMENTE." << endl; //Aquí se agrega un encabezado en el archivo de salida.
			entradaDatos >> lecturaNombre;

			while (lecturaNombre != "xxx") {

				entradaDatos >> puntuacionPartidas;
				salidaDatos << lecturaNombre << " "  << puntuacionPartidas << endl;
				entradaDatos >> lecturaNombre;
			}

			if (!encontrado) {
				salidaDatos << nombre << " " << puntuacion << endl;
			}

			salidaDatos << "xxx";
		}
	}

	else {
		salidaDatos << "Archivo de puntuaciones de Color Crush. NO MODIFICAR EXTERNAMENTE." << endl; //Aquí se agrega un encabezado en el archivo de salida.
		salidaDatos << nombre << " " << puntuacion << endl;
		salidaDatos << "xxx";
	}

	salidaDatos.close();
	entradaDatos.close();

	remove("ColorCrushData\\tmp.txt"); //Elimina el archivo tmp.txt
}


void colorCrush(string nombre, string fichero, bool carga, bool gelatina) {

	tJuego juego;
	tDireccion direccionAMover;
	int cuentaRepes = 0, desplazoDeF = 0, desplazoDeC = 0;
	bool salir = false, gel = gelatina;

	crearTablero(juego, gel, gelatina);
	
	if (carga) {
		cargarTablero(juego, fichero);
		Sleep(TIEMPO + 500);
	}
	else {
		cargarPartida(juego, gelatina);
	}

	while (juego.intentos >= 0 && !salir) {

		mostrarTablero(juego, gelatina);
		procesarTablero(juego, gel);
		
		if (gelatina && !gel) {
			salir = true;
		}
		if (juego.intentos > 0 && !salir) {
			salir = opcionUsuario(juego, desplazoDeF, desplazoDeC, direccionAMover);
			mciSendString ("play C:\\Windows\\Media\\chimes.wav", NULL, 0, NULL);
		}
		if (!salir) {
			moverTablero(juego.tablero, direccionAMover, desplazoDeF, desplazoDeC);
			juego.intentos--;
		}
	}
	guardarPartida(juego, gelatina, gel);
	guardarPuntuacionTemporal(nombre, juego.puntuacion);
}