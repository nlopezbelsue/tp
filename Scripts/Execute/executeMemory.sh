#!/bin/bash
# Obtener el directorio del script
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Obtener el directorio padre del script
parent_dir="$(dirname "$script_dir")"

# Obtener el directorio anterior al directorio padre
grandparent_dir="$(dirname "$parent_dir")"



export LD_LIBRARY_PATH="$grandparent_dir/Cliente-Servidor/Debug"


run_module() {
    module_name=$1
    config_file=$2

    cd "$grandparent_dir/$module_name/Debug"
    make all

    cd "$grandparent_dir"
   
    echo "Ejecutando el módulo $module_name con el archivo de configuración $config_file"
    
    valgrind --leak-check=full --log-file=memoriaVg.txt ./$module_name/Debug/$module_name ./$module_name/$config_file
}



run_module "Memory" "integral.config" 
