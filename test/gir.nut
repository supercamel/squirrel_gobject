print("here");

require("GLib", "2.0");

print("here\n");
MyLib <- {
    Rect = { }
}

class MyLib.Rect {
    constructor(a, b) {

    }

    testy = null
}


local rect = MyLib.Rect(1, 2);

GLib.get_processor_count();

function main() {
    print("here\n");
}

main();
