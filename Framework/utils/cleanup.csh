#!/bin/csh

echo "> Clean with ipcrm !"
set ipcs_list = `ipcs | cut -d " " -f 2`
set param=""
foreach item ($ipcs_list)
 if (($item == "Semaphore") || ($item == "Tableaux")) then 
    set param="-s"
 else if (param != "") then
    echo ipcs $param $item
    ipcrm $param $item
 endif 
end
