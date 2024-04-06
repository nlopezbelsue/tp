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
    nuevo_puerto_memoria=$4
    nuevo_puerto_escucha=$5
    

    if [ -z "$nueva_direccion" ]; then
        nueva_direccion="127.0.0.1"
        
    fi
    if [ -z "$nuevo_puerto_escucha" ]; then
        nuevo_puerto_escucha="8003"
       
       
    fi
    if [ -z "$nuevo_puerto_memoria" ]; then
        nuevo_puerto_memoria="8002"
       
    fi

    cd "$grandparent_dir/$module_name/Debug"
    make all

    cd "$grandparent_dir"
    sed -i "s/IP_MEMORIA=.*/IP_MEMORIA=$nueva_direccion/g" ./$module_name/$config_file
    sed -i "s/PUERTO_ESCUCHA=.*/PUERTO_ESCUCHA=$nuevo_puerto_escucha/g" ./$module_name/$config_file
    sed -i "s/PUERTO_MEMORIA=.*/PUERTO_MEMORIA=$nuevo_puerto_memoria/g" ./$module_name/$config_file

    echo "Utilizando la IP para memoria predeterminada: $nueva_direccion"
    echo "usando puerto escucha $nuevo_puerto_escucha"
    echo "usando puerto memoria $nuevo_puerto_memoria"

   
  
}



config_module "FileSystem" "FileSystem.config" "$1" "$2" "$3"
