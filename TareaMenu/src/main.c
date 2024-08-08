#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"

#define FILE_PATH "/spiffs/alumnos.dat"

typedef struct {
    char nombre[50];
    int edad;
    char carrera[50];
} Alumno;

// Funciones
void iniciar_spiffs();
void grabarDatos(Alumno **alumnos, int *cantidad);
void leerDatos(Alumno *alumnos, int cantidad);
void guardarEnDisco(Alumno *alumnos, int cantidad);
void cargarDesdeDisco(Alumno **alumnos, int *cantidad);
void liberarMemoria(Alumno *alumnos);

void app_main() {
    iniciar_spiffs();

    Alumno *alumnos = NULL;
    int cantidad = 0;
    int opcion;

    while (1) {
        printf("\nMenu:\n");
        printf("1. Grabar datos\n");
        printf("2. Leer datos\n");
        printf("3. Salir\n");
        printf("Seleccione una opción: ");

        // Leer la opción del usuario
        if (scanf("%d", &opcion) != 1) {
            printf("Error al leer la entrada.\n");
            while (getchar() != '\n'); // Limpiar el buffer en caso de error
            continue;
        }

        // Limpiar el buffer después de leer la opción
        while (getchar() != '\n'); 

        switch (opcion) {
            case 1:
                grabarDatos(&alumnos, &cantidad);
                guardarEnDisco(alumnos, cantidad);
                break;
            case 2:
                cargarDesdeDisco(&alumnos, &cantidad);
                leerDatos(alumnos, cantidad);
                break;
            case 3:
                printf("Saliendo...\n");
                liberarMemoria(alumnos);
                return;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
                break;
        }

        // Pausa para evitar que el menú se muestre de inmediato
        printf("Presione Enter para continuar...");
        while (getchar() != '\n'); // Limpiar el buffer
    }
}

void iniciar_spiffs() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Error: Fallo al montar el sistema de archivos SPIFFS.\n");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            printf("Error: No se encontró la partición SPIFFS.\n");
        } else {
            printf("Error: Fallo al iniciar SPIFFS (%s).\n", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        printf("Error: Fallo al obtener información de SPIFFS (%s).\n", esp_err_to_name(ret));
    } else {
        printf("SPIFFS montado correctamente. Tamaño total: %d bytes, Usado: %d bytes\n", total, used);
    }
}

void grabarDatos(Alumno **alumnos, int *cantidad) {
    char continuar;
    char buffer[50]; // Buffer para la entrada del usuario
    do {
        (*cantidad)++;
        *alumnos = (Alumno *)realloc(*alumnos, (*cantidad) * sizeof(Alumno));
        if (*alumnos == NULL) {
            printf("Error al asignar memoria.\n");
            exit(1);
        }
        printf("Ingrese nombre: ");
        fgets((*alumnos)[*cantidad - 1].nombre, 50, stdin);
        (*alumnos)[*cantidad - 1].nombre[strcspn((*alumnos)[*cantidad - 1].nombre, "\n")] = 0; // Elimina el salto de línea
        printf("Ingrese edad: ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            (*alumnos)[*cantidad - 1].edad = atoi(buffer);
        }
        printf("Ingrese carrera: ");
        fgets((*alumnos)[*cantidad - 1].carrera, 50, stdin);
        (*alumnos)[*cantidad - 1].carrera[strcspn((*alumnos)[*cantidad - 1].carrera, "\n")] = 0; // Elimina el salto de línea

        printf("¿Desea ingresar otro registro? (s/n): ");
        scanf(" %c", &continuar);
        while (getchar() != '\n'); // Limpiar el buffer después de scanf
    } while (continuar == 's');
}

void leerDatos(Alumno *alumnos, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        printf("\nAlumno %d:\n", i + 1);
        printf("Nombre: %s\n", alumnos[i].nombre);
        printf("Edad: %d\n", alumnos[i].edad);
        printf("Carrera: %s\n", alumnos[i].carrera);
    }
}

void guardarEnDisco(Alumno *alumnos, int cantidad) {
    FILE *archivo = fopen(FILE_PATH, "wb");
    if (archivo == NULL) {
        printf("Error al abrir el archivo para escribir.\n");
        return;
    }
    fwrite(&cantidad, sizeof(int), 1, archivo);
    fwrite(alumnos, sizeof(Alumno), cantidad, archivo);
    fclose(archivo);
    printf("Datos guardados en disco.\n");
}

void cargarDesdeDisco(Alumno **alumnos, int *cantidad) {
    FILE *archivo = fopen(FILE_PATH, "rb");
    if (archivo == NULL) {
        printf("Error al abrir el archivo para leer.\n");
        return;
    }
    fread(cantidad, sizeof(int), 1, archivo);
    *alumnos = (Alumno *)realloc(*alumnos, (*cantidad) * sizeof(Alumno));
    if (*alumnos == NULL) {
        printf("Error al asignar memoria.\n");
        exit(1);
    }
    fread(*alumnos, sizeof(Alumno), *cantidad, archivo);
    fclose(archivo);
    printf("Datos cargados desde el disco.\n");
}

void liberarMemoria(Alumno *alumnos) {
    free(alumnos);
}
