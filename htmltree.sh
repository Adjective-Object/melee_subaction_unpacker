#!/bin/bash
### create simple HTML format using a 'HERE document'
function create_html {
cat << EOM
<li><a href="${MY_FILE}">$(basename $MY_FILE .html)</a></li>
EOM
}
###
for MY_FILE in $(ls $1)
do
    create_html
done
# end of script

