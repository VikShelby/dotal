shpall miqte = ["Artan", "Besa", "Klea"];
printo "Miku im i pare eshte:";
printo miqte[0];

printo "Do te ndryshoj mikun e trete...";
miqte[2] = "Dritan";

shpall i = 0;
derisa (i < 3) {
  printo miqte[i];
  i = i + 1;
}

printo "Lista e plote eshte:";
printo miqte;