// AUTOR: BAA4TS
// ARCHIVO: GET.cpp
// LICENCIA: MIT
// FECHA DE CREACIÓN: 7 de octubre de 2024

// Librerías
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <tchar.h>

#pragma comment(lib, "wininet.lib")

class GET
{
private:
    int BUFFER_SIZE; // Definir un tamaño fijo para el buffer
    char *BUFFER;    // Usar puntero para el buffer
    bool console_logs;

    // Función para extraer el nombre de archivo de la URL
    std::string extraer_nombre(const std::string &url)
    {
        size_t ultimo_slash = url.find_last_of('/');
        return (ultimo_slash != std::string::npos) ? url.substr(ultimo_slash + 1) : "";
    }

    // Función para mostrar logs en consola
    void console_logs_func(int A, const char *logs, const std::string &Archivo, double descargado)
    {
        if (A == 1)
        {
            system("cls");
            std::cout << "[Archivo: (" << Archivo << ")]\nDescargado [ " << descargado << " ]\n";
        }
        else
        {
            std::cout << logs << std::endl;
        }
    }

    // Método para descargar el archivo desde la URL
    bool descargar(const std::string &url, const std::string &salida)
    {
        DWORD Bytes;

        if (url.empty())
        {
            if (console_logs)
                console_logs_func(0, "La URL está vacía.\n", "", 0);
            OutputDebugString(_T("La URL está vacía.\n"));
            return false;
        }

        HINTERNET hInternet = InternetOpenA("GET", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet)
        {
            OutputDebugString(_T("Error en el InternetOpen()"));
            if (console_logs)
                console_logs_func(0, "Error en el InternetOpen()", "", 0);
            return false;
        }

        HINTERNET hConneccion = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
        if (!hConneccion)
        {
            OutputDebugString(_T("Error en el InternetOpenUrl()"));
            if (console_logs)
                console_logs_func(0, "Error en el InternetOpenUrl()", "", 0);
            InternetCloseHandle(hInternet);
            return false;
        }

        // Asignar memoria para el buffer
        BUFFER = new char[BUFFER_SIZE];
        if (!BUFFER)
        {
            OutputDebugString(_T("Error al asignar memoria para el buffer.\n"));
            if (console_logs)
                console_logs_func(0, "Error al asignar memoria para el buffer.\n", "", 0);
            InternetCloseHandle(hConneccion);
            InternetCloseHandle(hInternet);
            return false;
        }

        std::ofstream Archivo(salida, std::ios::binary);
        if (!Archivo)
        {
            OutputDebugString(_T("Error al crear el archivo"));
            if (console_logs)
                console_logs_func(0, "Error al crear el archivo", "", 0);
            delete[] BUFFER;
            InternetCloseHandle(hConneccion);
            InternetCloseHandle(hInternet);
            return false;
        }

        int byte = 0; // Iniciar contador de bytes
        while (InternetReadFile(hConneccion, BUFFER, BUFFER_SIZE, &Bytes) && Bytes != 0)
        {
            byte += Bytes; // Actualizar el contador de bytes
            if (console_logs)
                console_logs_func(1, "", salida, static_cast<double>(byte) / 1024 / 1024); // Mostrar el progreso en MB
            Archivo.write(BUFFER, Bytes);
        }

        Archivo.close();
        InternetCloseHandle(hConneccion);
        InternetCloseHandle(hInternet);
        delete[] BUFFER;

        OutputDebugString(_T("Descarga iniciada con éxito.\n"));
        return true;
    }

public:
    GET(int buffer_size, bool console_logs)
        : BUFFER_SIZE(buffer_size), console_logs(console_logs), BUFFER(nullptr) {}

    bool descargarArchivo(const std::string &url, const std::string &salida = "")
    {
        std::string nombre_salida = salida.empty() ? extraer_nombre(url) : salida;
        return descargar(url, nombre_salida);
    }
};

int main()
{
    // Definir el tamaño del buffer y si se desean logs en consola
    const int BUFFER_SIZE = 1024; // 1 KB
    bool console_logs = true;     // Activar logs en consola

    // Crear una instancia de la clase GET
    GET downloader(BUFFER_SIZE, false);

    // URL del archivo a descargar
    std::string url = "https://mirrors.ocf.berkeley.edu/blender/release/Blender4.2/blender-4.2.2-windows-x64.msi"; // Reemplazar con la URL real

    // Llamar al método para descargar el archivo
    if (downloader.descargarArchivo(url, ""))
    {
        std::cout << "Descarga completada con éxito.\n";
    }
    else
    {
        std::cout << "Error en la descarga.\n";
    }

    return 0;
}