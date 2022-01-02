#!/usr/bin/python3

import gi
gi.require_version('squirrel', '0.1')
from gi.repository import squirrel 


def test_foo(hvm, usrdata):
    vm = squirrel.Vm.from_hvm(hvm)

    print(vm.get_top())
    vm.push_string("Hello world")
    return 1

def on_print(vm, str):
    print(str)


vm = squirrel.Vm.new(1024)
vm.connect("on_print", on_print)

vm.push_root_table()

vm.push_root_table()
vm.push_string("MyClass")
vm.new_class(False)

vm.push_string("test_foo")
vm.new_closure(test_foo, 0)
vm.new_slot(-3, False)

vm.push_string("thing")
vm.push_int(0)
vm.new_slot(-3, False)

vm.new_slot(-3, False)
vm.pop(1)

print("doing file")
vm.do_file("hello.nut", False, True)
