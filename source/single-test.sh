#!/bin/bash
ERROR_FILE="/tmp/single-test-error.txt"
DATA_DIR="/tmp/single-test"
LOOP_MAX="3000"
PUSH_CMD=$1
POP_CMD=$2

if [[ -d Debug ]];then
    PUSH_CMD="Debug/$PUSH_CMD"
    POP_CMD="Debug/$POP_CMD"
else
    if [[ -d Release ]];then
        PUSH_CMD="Release/$PUSH_CMD"
        POP_CMD="Release/$POP_CMD"
    fi
fi

if [[ -x $PUSH_CMD ]]; then
    echo "Push cmd: $PUSH_CMD $DATA_DIR"
else
    echo "$PUSH_CMD command not found (1)!"
    exit 1
fi

if [[ -x $POP_CMD ]]; then
    echo "Pop cmd: $POP_CMD $DATA_DIR"
else
    echo "$POP_CMD command not found (2)!"
    exit 2
fi

rm -f $ERROR_FILE
rm -Rf $DATA_DIR
mkdir -p $DATA_DIR

push_loop () {
    echo "push_loop started"
    for (( i = 0 ; i < $LOOP_MAX ; i++ )); do
        echo -n $i | $PUSH_CMD $DATA_DIR
    done
    echo "push_loop done"
}

pop_loop () {
    RESULT="OK"
    echo "pop_loop started"
    for (( i = 0 ; i < $LOOP_MAX ; i++ )); do
        ITEM=`$POP_CMD $DATA_DIR`
        if [[ "$i" != "$ITEM" ]];then
            echo "Expected $i but $ITEM got instead..." >> $ERROR_FILE
            RESULT="NG"
        fi
    done
    echo "pop_loop done - result: $RESULT"
}

push_loop &
sleep 1
pop_loop &
wait

if [[ -r $ERROR_FILE ]]; then
    cat $ERROR_FILE
    exit 3
fi
exit 0