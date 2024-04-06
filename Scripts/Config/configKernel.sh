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
    nueva_ip_fs=$5
    nuevo_puerto_fs=$6
    nueva_ip_cpu=$7
    nuevo_puerto_cpuDis=$8
    nuevo_puerto_cpuInt=$9

    if [ $# -eq 0 ]; then

        nueva_direccion="127.0.0.1"
        echo "Utilizando la IP para memoria predeterminada: $nueva_direccion"

        nuevo_puerto_memoria="8002"
        echo "usando puerto memoria $nuevo_puerto_memoria"

        nueva_ip_fs="127.0.0.1"
        echo "usando IP para filesystem $nueva_ip_fs"

        nuevo_puerto_fs="8003"
        echo "usando puerto filesystem $nuevo_puerto_fs"

        nueva_ip_cpu="127.0.0.1"
        echo "usando ip cpu $nueva_ip_cpu"

        nuevo_puerto_cpuDis="8006"
        echo "usando puerto cpu dispatch $nuevo_puerto_cpuDis"

        nuevo_puerto_cpuInt="8007"
        echo "usando puerto cpu interrupt $nuevo_puerto_cpuInt"


    fi

    cd "$grandparent_dir/$module_name/Debug"
    make all

    cd "$grandparent_dir"


    sed -i "s/IP_FILESYSTEM=.*/IP_FILESYSTEM=$nueva_direccion/g" ./$module_name/$config_file
    
    sed -i "s/PUERTO_MEMORIA=.*/PUERTO_MEMORIA=$nuevo_puerto_memoria/g" ./$module_name/$config_file
    
    sed -i "s/IP_FILESYSTEM=.*/IP_FILESYSTEM=$nueva_ip_fs/g" ./$module_name/$config_file
    
    sed -i "s/PUERTO_FILESYSTEM=.*/PUERTO_FILESYSTEM=$nuevo_puerto_fs/g" ./$module_name/$config_file
    
    sed -i "s/IP_CPU=.*/IP_CPU=$nueva_ip_cpu/g" ./$module_name/$config_file
    
    sed -i "s/PUERTO_CPU_DISPATCH=.*/PUERTO_CPU_DISPATCH=$nuevo_puerto_cpuDis/g" ./$module_name/$config_file
    
    sed -i "s/PUERTO_CPU_INTERRUPT=.*/PUERTO_CPU_INTERRUPT=$nuevo_puerto_cpuInt/g" ./$module_name/$config_file

    
}






config_module "Kernel" "kernel.config" "$1" "$2" "$3" "$4" "$5" "$6" "$7"
