fn main()
{
    var a := ([10, 9, 8, 7, 6], [5, 4, 3, 2, 1]);
    var b := a.0;
    var c := a.1;
    var d := *(b + 0);
    var e := *(c + 0);
    var f := d + e;
    f;
};