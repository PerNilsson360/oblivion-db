#! /bin/bash
function test() {
    echo '{}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '{"a":null}' | \
        ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    return 0
}
test
