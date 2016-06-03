client=./client

if [ ! -e $client ]; then
    make -C client
fi

i=0
while [ 0 ]; do
    i=$(expr $i + 1)
    $client localhost hi 10000 2>&1 > /dev/null
    if [ "$?" -eq "0" ]; then
        echo "request $i successful"
    else
        echo "request $i failed"
    fi
    sleep 0.5
done

