#!/usr/bin/python3

import gi
gi.require_version('squirrel', '0.1')
from gi.repository import squirrel 


def on_print(vm, str):
    print(str)


vm = squirrel.Vm.new(1024)
vm.connect("on_print", on_print)

#vm.push_root_table()

#vm.push_string("test_foo")
#vm.new_closure(test_foo, None, 0)
#vm.new_slot(-3, False)

vm.do_file("gir.nut", False, True)
