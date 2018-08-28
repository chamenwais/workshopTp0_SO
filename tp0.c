#include "tp0.h"

int main() {
	configure_logger();
	int socket = connect_to_server(IP, PUERTO);
	wait_hello(socket);
	Alumno alumno = read_hello();
	send_hello(socket, alumno);
	void * content = wait_content(socket);
	send_md5(socket, content);
	wait_confirmation(socket);
	exit_gracefully(0);
}

void configure_logger() {
	/*
	 1.  Creemos el logger con la funcion de las commons log_create.
	 Tiene que: guardarlo en el archivo tp0.log, mostrar 'tp0' al loggear,
	 mostrarse por pantalla y mostrar solo los logs de nivel info para arriba
	 (info, warning y error!)
	 */
	logger = log_create("/home/utnso/Logs/tp0.log", "tp0", 1, LOG_LEVEL_INFO);
}

void validarRetornoConexion(int retorno, int socket) {
	/*
	 3.1 Recuerden chequear por si no se pudo contectar (usando el retorno de connect()).
	 Si hubo un error, lo loggeamos y podemos terminar el programa con la funcioncita
	 exit_gracefully pasandole 1 como parametro para indicar error ;).
	 Pss, revisen los niveles de log de las commons.
	 */
	if (retorno < 0) {
		log_error(logger, "No se pudo conectar la ip");
		close(socket);
		exit_gracefully(1);
	}
}

void validarRetornoMensaje(int retorno) {
	if (retorno < 0) {
		log_error(logger, "No se se obtuvo ningun mensaje");
	}
}

void validarLoRecibido(char * recibido, char * hola) {
	if (!string_equals_ignore_case(recibido, hola)) {
		log_info(logger, "No recibio el hola");
	}
}

int connect_to_server(char * ip, char * port) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;  // Indica que usaremos el protocolo TCP

	getaddrinfo(ip, port, &hints, &server_info); // Carga en server_info los datos de la conexion

	// 2. Creemos el socket con el nombre "server_socket" usando la "server_info" que creamos anteriormente
	int server_socket = socket(server_info->ai_family, server_info->ai_socktype,
			server_info->ai_protocol);

	// 3. Conectemosnos al server a traves del socket! Para eso vamos a usar connect()
	int retorno = connect(server_socket, server_info->ai_addr,
			server_info->ai_addrlen);

	freeaddrinfo(server_info);  // No lo necesitamos mas

	/*
	 3.1 Recuerden chequear por si no se pudo contectar (usando el retorno de connect()).
	 Si hubo un error, lo loggeamos y podemos terminar el programa con la funcioncita
	 exit_gracefully pasandole 1 como parametro para indicar error ;).
	 Pss, revisen los niveles de log de las commons.
	 */
	validarRetornoConexion(retorno, server_socket);

	// 4 Logeamos que pudimos conectar y retornamos el socket
	log_info(logger, "Conectado!");
	return server_socket;
}

void wait_hello(int socket) {
	char * mensaje_a_recibir = "SYSTEM UTNSO 0.1";

	/*
	 5.  Ya conectados al servidor, vamos a hacer un handshake!
	 Para esto, vamos a, primero recibir un mensaje del
	 servidor y luego mandar nosotros un mensaje.
	 Deberìamos recibir lo mismo que está contenido en la
	 variable "hola". Entonces, vamos por partes:
	 5.1.  Reservemos memoria para un buffer para recibir el mensaje.
	 */
	size_t holaBinaryLengh = strlen(mensaje_a_recibir)+1;
	void * buffer = malloc(holaBinaryLengh);
	/*
	 5.2.  Recibamos el mensaje en el buffer.
	 Recuerden el prototipo de recv:
	 conexión - donde guardar - cant de bytes - flags(si no se pasa ninguno puede ir NULL)
	 Nota: Palabra clave MSG_WAITALL.
	 */
	int result_recv = recv(socket, buffer, holaBinaryLengh, NULL);
	/*
	 5.3.  Chequiemos errores al recibir! (y logiemos, por supuesto)
	 5.4.  Comparemos lo recibido con "hola".
	 Pueden usar las funciones de las commons para comparar!
	 No se olviden de loggear y devolver la memoria que pedimos!
	 (si, también si falló algo, tenemos que devolverla, atenti.)
	 */
	validarRetornoMensaje(result_recv);
	printf("Nos devolvio %s \n", buffer);
	validarLoRecibido(buffer, mensaje_a_recibir);

	free(buffer);
}

Alumno read_hello() {
	/*
	 6.    Ahora nos toca mandar a nosotros un mensaje de hola.
	 que van a ser nuestros datos, definamos una variable de tipo Alumno.
	 Alumno es esa estructura que definimos en el .h.
	 Recuerden definir el nombre y apellido como cadenas varias, dado
	 que como se va a enviar toda la estructura completa, para evitar problemas
	 con otros otros lenguajes
	 */
	Alumno alumno = { .nombre = "", .apellido = "" };

	/*
	 7.    Pero como conseguir los datos? Ingresemoslos por consola!
	 Para eso, primero leamos por consola usando la biblioteca realine.
	 Vamos a recibir, primero el legajo, despues el nombre y
	 luego el apellido
	 */
	char * legajo = readline("Legajo: ");

	/*
	 8.    Realine nos va a devolver un cacho de memoria ya reservada
	 con lo que leyo del teclado hasta justo antes del enter (/n).
	 Ahora, nos toca copiar el legajo al de la estructura alumno. Como
	 el legajo es numero, conviertanlo a numero con la funcion atoi
	 y asignenlo.
	 Recuerden liberar la memoria pedida por readline con free()!
	 */

	alumno.legajo = atoi(legajo);
	free(legajo);

	/*
	 9.    Para el nombre y el apellido no hace falta convertirlos porque
	 ambos son cadenas de caracteres, por los que solo hace falta
	 copiarlos usando memcpy a la estructura y liberar la memoria
	 pedida por readline.
	 */
	// Usemos memcpy(destino, origen, cant de bytes).
	char * nombre = readline("Nombre: ");
	char * apellido = readline("Apellido: ");
	memcpy(alumno.nombre, nombre, strlen(nombre) + 1);
	// Para la cant de bytes nos conviene usar strlen dado que son cadenas
	// de caracteres que cumplen el formato de C (terminar en \0)
	// 9.1. Faltaría armar el del apellido
	memcpy(alumno.apellido, apellido, strlen(apellido) + 1);
	// 10. Finalmente retornamos la estructura
	return alumno;
}

void validarRetornoEnvio(int resultado, int socket) {
	if (resultado <= 0) {
		/*
		 12.1. Recuerden que si hay error, hay que salir y tenemos que cerrar el socket (ademas de loggear)!
		 */
		log_error(logger, "El mensaje no se pudo enviar");
		close(socket);
		exit_gracefully(1);
	}
}

void send_hello(int socket, Alumno alumno) {
	log_info(logger, "Enviando info de Estudiante");
	/*
	 11.   Ahora SI nos toca mandar el hola con los datos del alumno.
	 Pero nos falta algo en nuestra estructura, el id_mensaje del protocolo.
	 Segun definimos, el tipo de id para un mensaje de tamaño fijo con
	 la informacion del alumno es el id 99
	 */
	alumno.id_mensaje = 99;
	/*
	 11.1. Como algo extra, podes probar enviando caracteres invalidos en el nombre
	 o un id de otra operacion a ver que responde el servidor y como se
	 comporta nuestro cliente.
	 */

	// alumno.id_mensaje = 33;
	// alumno.nombre[2] = -4;
	/*
	 12.   Finalmente, enviemos la estructura por el socket!
	 Recuerden que nuestra estructura esta definida como __attribute__((packed))
	 por lo que no tiene padding y la podemos mandar directamente sin necesidad
	 de un buffer y usando el tamaño del tipo Alumno!
	 */

	int resultado = send(socket, &alumno, sizeof(Alumno), 0);

	validarRetornoEnvio(resultado, socket);
	return;
}

char * conocerTipoDeContenido(ContentHeader *header) {
	if (header->id == 18) {
		return "variable";
	}
	return "no variable";
}

bool esDeContenidoVariable(char* tipoDeContenido) {
	return string_equals_ignore_case(tipoDeContenido, "variable");
}

void liberarMemoriaDeWaitContent(void **buf, ContentHeader** header) {
	/*
	 15.   Finalmente, no te olvides de liberar la memoria que pedimos
	 para el header y retornar el contenido recibido.
	 */
	free(header);
	free(buf);
}

void validarPosibleErrorDeContenido(int result_recv_content, ContentHeader *header, int socket, void * buff) {
	if (result_recv_content <= 0) {
		free(header);
		log_error(logger,
				"Error obteniendo el contenido de la respuesta del servidor");
		close(socket);
		exit_gracefully(1);
	}
}

void * wait_content(int socket) {
	/*
	 13.   Ahora tenemos que recibir un contenido de tamaño variable
	 Para eso, primero tenemos que confirmar que el id corresponde al de una
	 respuesta de contenido variable (18) y despues junto con el id de operacion
	 vamos a haber recibido el tamaño del contenido que sigue. Por lo que:
	 */
	size_t contentHeaderSize = sizeof(ContentHeader);

	log_info(logger, "Esperando el encabezado del contenido(%ld bytes)",
			contentHeaderSize);
	// 13.1. Reservamos el suficiente espacio para guardar un ContentHeader
	ContentHeader * header = malloc(contentHeaderSize);

	// 13.2. Recibamos el header en la estructura y chequiemos si el id es el correcto.
	//      No se olviden de validar los errores, liberando memoria y cerrando el socket!
	int result_recv_header = recv(socket, header, contentHeaderSize, MSG_WAITALL);

	validarRetornoMensaje(result_recv_header);
	char* tipoDeContenido = conocerTipoDeContenido(header);
	log_info(logger, "El contenido del mensaje es de tipo %s", tipoDeContenido);
	int responseLenght = header->len;
	log_info(logger, "Esperando el contenido (%d bytes)", responseLenght);

	/*
	 14.   Ahora, recibamos el contenido variable. Ya tenemos el tamaño,
	 por lo que reecibirlo es lo mismo que veniamos haciendo:
	 14.1. Reservamos memoria */
	/*14.2. Recibimos el contenido en un buffer (si hubo error, fallamos, liberamos y salimos
	 */
	void * buf;
	int result_recv_content = -1;

	if (esDeContenidoVariable(tipoDeContenido)) {
		buf = malloc(responseLenght+2);
		result_recv_content = recv(socket, buf, responseLenght, MSG_WAITALL);
	} else {
		buf = malloc(62);
		result_recv_content = recv(socket, buf, 60, MSG_WAITALL);
	}

	validarRetornoMensaje(result_recv_content);
	validarPosibleErrorDeContenido(result_recv_content, header, socket, buf);

	free(header);

	//TODO reveer
	string_append(buf, " ");
	log_info(logger, "Se recibio la siguiente respuesta a los datos del alumno %s",
				buf);
	return buf;
}

void validarLiberacionBuffer(int result, void * buffer){
	if(result<=0){
		free(buffer);
	}
}

void send_md5(int socket, void * content) {
	/*
	 16.   Ahora calculemos el MD5 del contenido, para eso vamos
	 a armar el digest:
	 */

	void * digest = malloc(MD5_DIGEST_LENGTH);
	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context, content, strlen(content) + 1);
	MD5_Final(digest, &context);

	free(content);

	/*
	 17.   Luego, nos toca enviar a nosotros un contenido variable.
	 A diferencia de recibirlo, para mandarlo es mejor enviarlo de una,
	 siguiendo la logica de 1 send - N recv.
	 Asi que:
	 */

	//      17.1. Creamos un ContentHeader para guardar un mensaje de id 33 y el tamaño del md5
	size_t contentHeaderSize = sizeof(ContentHeader);
	size_t totalSize = MD5_DIGEST_LENGTH + contentHeaderSize;
	ContentHeader header = { .id = 33, .len = totalSize };
	/*
	 17.2. Creamos un buffer del tamaño del mensaje completo y copiamos el header y la info de "digest" allí.
	 Recuerden revisar la función memcpy(ptr_destino, ptr_origen, tamaño)!
	 */
	void * buf = malloc(totalSize);
	memcpy(&buf, &header, contentHeaderSize);
	memcpy(&buf, &digest, MD5_DIGEST_LENGTH);

	/*
	 18.   Con lo anterior listo, solo nos falta enviar el paquete que armamos y liberar la memoria que usamos.
	 Si, TODA la que usamos, eso incluye a la del contenido del mensaje que recibimos en la función
	 anterior y el digest del MD5. Obviamente, validando tambien los errores.
	 */
	int resultado = send(socket, buf, totalSize, 0);

	validarLiberacionBuffer(resultado, buf);
	validarRetornoEnvio(resultado, socket);
	free(buf);
}

int* castVoidPtrToIntPtr(void* resultBuf) {
	size_t intSize = sizeof(int);
	int* castedResult = malloc(intSize);
	memcpy(&castedResult, &resultBuf, intSize);
	free(resultBuf);
	return castedResult;
}

void wait_confirmation(int socket) {
	int result = 0; // Dejemos creado un resultado por defecto
	size_t resultSize = sizeof(result);
	void * resultBuf=malloc(resultSize);
	/*
	 19.   Ahora nos toca recibir la confirmación del servidor.
	 Si el resultado obtenido es distinto de 1, entonces hubo un error
	 */

	int result_recv_header = recv(socket, resultBuf, resultSize, MSG_WAITALL);

	validarRetornoMensaje(result_recv_header);
	int* castedVoidPtr = castVoidPtrToIntPtr(resultBuf);
	if (*castedVoidPtr == 1) {
		log_info(logger, "Los MD5 concidieron!");
	} else {
		log_error(logger, "Hubo un error con los MD5!");
	}
	free(castedVoidPtr);
}

void exit_gracefully(int return_nr) {
	/*
	 20.   Siempre llamamos a esta funcion para cerrar el programa.
	 Asi solo necesitamos destruir el logger y usar la llamada al
	 sistema exit() para terminar la ejecucion
	 */
	log_destroy(logger);
	exit(return_nr);
}
