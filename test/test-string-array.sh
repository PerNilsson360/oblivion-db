#! /bin/bash
function test() {
    echo '[]' | ../src/jsch --schema=string-array-schema.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '["foo","bar"]' | ../src/jsch --schema=string-array-schema.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '["foo","bar",1]' | ../src/jsch --schema=string-array-schema.json
    if [ $? -ne 255 ]; then
        return 255
    fi
    echo '{}' | ../src/jsch --schema=string-array-schema.json
    if [ $? -ne 255 ]; then
        return 255
    fi

    return 0
}
test
