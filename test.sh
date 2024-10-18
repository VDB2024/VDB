#!/bin/bash

# 设置当前目录和目标目录
search_dir="."
destination_dir="./add/log"
new_filename="log"


mkdir -p "$destination_dir"


latest_file=$(find "$search_dir" -maxdepth 1 -type f -name "log_*" -printf "%T@ %p\n" | sort -nr | head -n 1 | cut -d' ' -f2-)

if [[ -n $latest_file ]]; then
    echo "Copying latest file: $latest_file"
    cp "$latest_file" "$destination_dir/$new_filename"
    echo "File copied and renamed to $new_filename at $destination_dir"
else
    echo "No matching files found."
fi


find "$search_dir" -maxdepth 1 -type f -name "log_*" -exec rm {} \;
