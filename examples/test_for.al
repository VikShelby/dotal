# Testimi i ciklit 'per'

printo "Numerimi nga 0 deri ne 4:";
per (shpall i = 0; i < 5; i = i + 1) {
  printo i;
}

printo "Llogaritja e shumes se numrave nga 1 deri ne 10:";
shpall shuma = 0;
per (shpall n = 1; n <= 10; n = n + 1) {
  shuma = shuma + n;
}
printo shuma; # Duhet te jete 55