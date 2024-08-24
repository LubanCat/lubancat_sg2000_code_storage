while true
do
    devmem 0x05026020 w 6
    sleep 1
    devmem 0x05026020 w 7
    sleep 1
done

