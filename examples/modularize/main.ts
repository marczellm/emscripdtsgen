/// <reference path="../../build/ModuleFactory.d.ts" />

let anyError = false;
let testsPassed = 0;

function assert(value: unknown, msg?: string) {
    anyError = anyError || !value;
    testsPassed += value ? 1 : 0;
    console.assert(!!value, msg);
}

function assertEq (actual: unknown, expected: unknown) {
    assert (actual == expected, `actual ${actual} does not equal expected ${expected}`);
}

ModuleFactory().then(Module => {
    assertEq(Module.function0(), 0);
    assertEq(Module.function1(1), 1);
    assertEq(Module.function2(1, 2), 3);

    assertEq(Module.konstans, "constant");

    const obj1 = Module.Class1.Create();
    const obj2 = new Module.Class2("name");
    
    assert(obj1);
    assert(obj2);

    assertEq(obj1.mChar, "c".charCodeAt(0));
    assertEq(obj1.mField, 5);
    
    assert(! obj2.get_class1({
        mBool: false,
        mEnum: Module.Enum.Value3
    }));
    assert(! obj2.get_class2(obj1));

    const struct1 = obj1.get_struct1();
    assert(struct1.mBool);
    assertEq(struct1.mEnum, Module.Enum.Value2);

    const struct2 = obj1.get_struct2();
    const struct1b = struct2.mStruct;
    assert(struct1b.mBool);
    assertEq(struct1b.mEnum, Module.Enum.Value2);

    obj1.delete();
    obj2.delete();

    console.log(anyError ? 'Test failed' : (testsPassed + ' tests passed'));
});
