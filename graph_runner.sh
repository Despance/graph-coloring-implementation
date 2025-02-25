#!/bin/bash

# Klasör yolu
DATASET_DIR="incea-master/datasets/"

# Dosyaları bul ve çalıştır
find "$DATASET_DIR" -type f -name "*.col" | while read -r file; do
    echo "Processing: $file"
    ./graph_Coloring "$file"

    # Eğer işlem başarısız olursa hata mesajı ver
    if [ $? -ne 0 ]; then
        echo "Error processing $file"
    fi
done
