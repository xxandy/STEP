def foo(b):
    # b points to the same address as a (call by reference).
    print('id(b): %#08x' % id(b))  # 0x7f5241d82600
    b.append(2)
    # This updates b as well as a because list is mutable.

    # Assingning an object itself creates a new object.
    b = b + [3]
    # b no longer points the same address as a.
    print('id(b): %#08x' % id(b))  # 0x7f5241c11b40
    b.append(4)
    print('id(b): %#08x' % id(b))  # 0x7f5241c11b40
    print('b:', b)  # [1, 2, 3, 4]


a = [1]
print('id(a): %#08x' % id(a))  # 0x7f5241d82600
foo(a)
print('a:', a)  # [1, 2]
