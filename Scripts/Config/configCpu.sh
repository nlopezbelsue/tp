# Obtener el directorio del script
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Obtener el directorio padre del script
parent_dir="$(dirname "$script_dir")"

# Obtener el directorio anterior al directorio padre
grandparent_dir="$(dirname "$parent_dir")"





# Configuración general
export LD_LIBRARY_PATH="$grandparent_dir/Cliente-Servidor/Debug"


config_module() {
    module_name=$1
    config_file=$2
    nueva_direccion=$3
    nuevo_puerto_memoria=$3
    puerto_escucha_dispatch=$4
    puerto_escucha_interrupt=$5
    

    if [ -z "$nueva_direccion" ]; then
        nueva_direccion="127.0.0.1"
        
    fi
    if [ -z "$nuevo_puerto_memoria" ]; then
        nuevo_puerto_memoria="8002"
        
    fi
    if [ -z "$puerto_escucha_dispatch" ]; then
        puerto_escucha_dispatch="8006"
       
    fi
    if [ -z "$puerto_escucha_interrupt" ]; then
        puerto_escucha_interrupt="8007"
        
    fi
    cd "$grandparent_dir/$module_name/Debug"
    make all

    cd "$grandparent_dir"
    sed -i "s/IP_MEMORIA=.*/IP_MEMORIA=$nueva_direccion/g" ./$module_name/$config_file
    sed -i "s/PUERTO_MEMORIA=.*/PUERTO_MEMORIA=$nuevo_puerto_memoria/g" ./$module_name/$config_file
    sed -i "s/PUERTO_ESCUCHA_DISPATCH=.*/PUERTO_ESCUCHA_DISPATCH=$puerto_escucha_dispatch/g" ./$module_name/$config_file
    sed -i "s/PUERTO_ESCUCHA_INTERRUPT=.*/PUERTO_ESCUCHA_INTERRUPT=$puerto_escucha_interrupt/g" ./$module_name/$config_file

    echo "Utilizando la IP para memoria $nueva_direccion"
    echo "usando puerto memoria $nuevo_puerto_memoria"
    echo "usando puerto escucha dispatch $puerto_escucha_dispatch"
    echo "usando puerto escucha dispatch $puerto_escucha_interrupt"
}


# Ejecutar el modulo con su configuracion respectiva y el argumento proporcionado
config_module "CPU" "CPU.config" "$1" "$2" "$3" "$4"