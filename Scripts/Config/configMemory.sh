# Obtener el directorio del script
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Obtener el directorio padre del script
parent_dir="$(dirname "$script_dir")"

# Obtener el directorio anterior al directorio padre
grandparent_dir="$(dirname "$parent_dir")"



# Configuración general
export LD_LIBRARY_PATH="$grandparent_dir/Cliente-Servidor/Debug"

# Directorio relativo al script
directorio=$(realpath "$0")
directorioEjecutable=$(dirname "$directorio")
directorioModulo=$(dirname "$directorioEjecutable")
directorioTp=$(dirname "$directorioModulo")


config_module() {
    module_name=$1
    config_file=$2
    nueva_direccion=$3
    nuevo_puerto_escucha=$4
    nuevo_puerto_fs=$5

     if [ -z "$nueva_direccion" ]; then
        nueva_direccion="127.0.0.1"
        echo "Utilizando la IP para filesystem predeterminada: $nueva_direccion"
    fi
    if [ -z "$nuevo_puerto_escucha" ]; then
        nuevo_puerto_escucha="8002"
        echo "usando puerto escucha $nuevo_puerto_escucha"
       
    fi
    if [ -z "$nuevo_puerto_fs" ]; then
        nuevo_puerto_fs="8003"
        echo "usando puerto filesystem $nuevo_puerto_fs"
    fi


    cd "$grandparent_dir/$module_name/Debug"
    make all

    cd "$grandparent_dir"

    sed -i "s/IP_FILESYSTEM=.*/IP_FILESYSTEM=$nueva_direccion/g" ./$module_name/$config_file
    sed -i "s/PUERTO_ESCUCHA=.*/PUERTO_ESCUCHA=$nuevo_puerto_escucha/g" ./$module_name/$config_file
    sed -i "s/PUERTO_FILESYSTEM=.*/PUERTO_FILESYSTEM=$nuevo_puerto_fs/g" ./$module_name/$config_file


}




config_module "Memory" "memory.config" "$1" "$2" "$3"
