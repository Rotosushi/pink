fn main()
{
    x := y := 0;
    a := &x;
    b := &y;
    *a = 5;
    *b = 9;
    z := *a + *b;
    z;
};