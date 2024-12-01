#!/bin/bash

cd ./build
task_name=$1

# 通用参数
shell_default_args="-v -k --gdb --filesys-size=2"
kernel_default_args="-f -q"

# 如果有任务参数
declare -A task_args
task_args[args-single]="onearg"
task_args[args-multiple]="some arguments for you!"
task_args[args-many]="a b c d e f g h i j k l m n o p q r s t u v"
task_args[args-dbl-space]="two  spaces!"

task_args[multi-recurse]="15"

# 需要导入的文件
declare -A task_files
task_files[args-none]="tests/userprog/args-none"
task_files[args-single]="tests/userprog/args-single"
task_files[args-multiple]="tests/userprog/args-multiple"
task_files[args-many]="tests/userprog/args-many"
task_files[args-dbl-space]="tests/userprog/args-dbl-space"

task_files[sc-bad-sp]="tests/userprog/sc-bad-sp"
task_files[sc-bad-arg]="tests/userprog/sc-bad-arg"
task_files[sc-boundary]="tests/userprog/sc-boundary"
task_files[sc-boundary-2]="tests/userprog/sc-boundary-2"
task_files[sc-boundary-3]="tests/userprog/sc-boundary-3"

task_files[halt]="tests/userprog/halt"
task_files[exit]="tests/userprog/exit"

task_files[create-normal]="tests/userprog/create-normal"
task_files[create-empty]="tests/userprog/create-empty"
task_files[create-null]="tests/userprog/create-null"
task_files[create-bad-ptr]="tests/userprog/create-bad-ptr"
task_files[create-long]="tests/userprog/create-longr"
task_files[create-exists]="tests/userprog/create-exists"
task_files[create-bound]="tests/userprog/create-bound"

task_files[open-normal]="tests/userprog/open-normal, ../../tests/userprog/sample.txt"
task_files[open-missing]="tests/userprog/open-missing"
task_files[open-boundary]="tests/userprog/open-boundary, ../../tests/userprog/sample.txt"
task_files[open-empty]="tests/userprog/open-empty"
task_files[open-null]="tests/userprog/open-null"
task_files[open-bad-ptr]="tests/userprog/open-bad-ptr"
task_files[open-twice]="tests/userprog/open-twice, ../../tests/userprog/sample.txt"

task_files[close-normal]="tests/userprog/close-normal, ../../tests/userprog/sample.txt"
task_files[close-twice]="tests/userprog/close-twice, ../../tests/userprog/sample.txt"
task_files[close-stdin]="tests/userprog/close-stdin"
task_files[close-stdout]="tests/userprog/close-stdout"
task_files[close-bad-fd]="tests/userprog/close-bad-fd"

task_files[read-normal]="tests/userprog/read-normal, ../../tests/userprog/sample.txt"
task_files[read-bad-ptr]="tests/userprog/read-bad-ptr, ../../tests/userprog/sample.txt"
task_files[read-boundary]="tests/userprog/read-boundary, ../../tests/userprog/sample.txt"
task_files[read-zero]="tests/userprog/read-zero, ../../tests/userprog/sample.txt"
task_files[read-stdout]="tests/userprog/read-stdout"
task_files[read-bad-fd]="tests/userprog/read-bad-fd"

task_files[write-normal]="tests/userprog/write-normal, ../../tests/userprog/sample.txt"
task_files[write-bad-ptr]="tests/userprog/write-bad-ptr, ../../tests/userprog/sample.txt"
task_files[write-boundary]="tests/userprog/write-boundary, ../../tests/userprog/sample.txt"
task_files[write-zero]="tests/userprog/write-zero, ../../tests/userprog/sample.txt"
task_files[write-stdin]="tests/userprog/write-stdin"
task_files[write-bad-fd]="tests/userprog/write-bad-fd, ../../tests/userprog/sample.txt"

task_files[exec-once]="tests/userprog/exec-once, tests/userprog/child-simple"
task_files[exec-arg]="tests/userprog/exec-arg, tests/userprog/child-args"
task_files[exec-bound]="tests/userprog/exec-bound, tests/userprog/child-args"
task_files[exec-bound-2]="tests/userprog/exec-bound-2"
task_files[exec-bound-3]="tests/userprog/exec-bound-3"
task_files[exec-multiple]="tests/userprog/exec-multiple, tests/userprog/child-simple"
task_files[exec-missing]="tests/userprog/exec-missing"
task_files[exec-bad-ptr]="tests/userprog/exec-bad-ptr"

task_files[wait-simple]="tests/userprog/wait-simple, tests/userprog/child-simple"
task_files[wait-twice]="tests/userprog/wait-twice, tests/userprog/child-simple"
task_files[wait-killed]="tests/userprog/wait-killed, tests/userprog/child-bad"
task_files[wait-bad-pid]="tests/userprog/wait-bad-pid"

task_files[multi-recurse]="tests/userprog/multi-recurse"
task_files[multi-child-fd]="tests/userprog/multi-child-fd, tests/userprog/child-close"

task_files[rox-simple]="tests/userprog/rox-simple"
task_files[rox-child]="tests/userprog/rox-child, tests/userprog/child-rox"
task_files[rox-multichild]="tests/userprog/rox-multichild, tests/userprog/child-rox"

task_files[bad-read]="tests/userprog/bad-read"
task_files[bad-write]="tests/userprog/bad-write"
task_files[bad-read2]="tests/userprog/bad-read2"
task_files[bad-write2]="tests/userprog/bad-write2"
task_files[bad-jump]="tests/userprog/bad-jump"
task_files[bad-jump2]="tests/userprog/bad-jump2"

task_files[lg-create]="tests/filesys/base/lg-create"
task_files[lg-full]="tests/filesys/base/lg-full"
task_files[lg-random]="tests/filesys/base/lg-random"
task_files[lg-seq-block]="tests/filesys/base/lg-seq-block"
task_files[lg-seq-random]="tests/filesys/base/lg-seq-random"

task_files[sm-create]="tests/filesys/base/sm-create"
task_files[sm-full]="tests/filesys/base/sm-full"
task_files[sm-random]="tests/filesys/base/sm-random"
task_files[sm-seq-block]="tests/filesys/base/sm-seq-block"
task_files[sm-seq-random]="tests/filesys/base/sm-seq-random"

task_files[syn-read]="tests/filesys/base/syn-read, tests/filesys/base/child-syn-read"
task_files[syn-remove]="tests/filesys/base/syn-remove"
task_files[syn-write]="tests/filesys/base/syn-write, tests/filesys/base/child-syn-wrt"

# 判断是否在数组中 find [判断目标] [数组]
function find {
    local element="$1"
    shift
    local array=("$@")
    for item in "${array[@]}"; do
        if [[ "$item" == "$element" ]]; then
            return 0
        fi
    done
    return 1 
}
# 按,分割文件 split_files [字符串] [目标数组]
function split_files {
    local str="$1"
    local target_array="$2"
    IFS=","
    read -r -a temp <<< "$str"
    eval "$target_array=(\"\${temp[@]}\")"
}

kernel_run_args=""
file_import_args=""
# 生成导入文件参数
temp_files_array=""
split_files "${task_files[$task_name]}" "temp_files_array"
for path in "${temp_files_array[@]}"; do
    if [ -z "$path" ]; then
        echo "$task_name not found"
        exit 1
    fi
    file_import_args="$file_import_args -p $path -a $(basename "$path")"
done
# 生成运行参数
if find $task_name ${!task_args[@]}; then
    kernel_run_args="'$task_name ${task_args[$task_name]}'"
else
    kernel_run_args="$task_name"
fi
# 生成整个命令
run_cli="../../utils/pintos $shell_default_args $file_import_args -- $kernel_default_args run $kernel_run_args"
eval $run_cli
# 最后回显一下
echo ""
echo "Run Task Command:"
echo "../../utils/pintos $shell_default_args"
echo "$file_import_args"
echo "-- $kernel_default_args run $kernel_run_args"
