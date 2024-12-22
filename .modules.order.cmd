cmd_/home/rubin/sarvenaz_stuff/modules.order := {   echo /home/rubin/sarvenaz_stuff/specific.ko; :; } | awk '!x[$$0]++' - > /home/rubin/sarvenaz_stuff/modules.order
