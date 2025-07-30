# Gjuha ime DOTAL - Prova Finale

printo "--- Testimi i Funksioneve dhe Cikleve ---";

funksion fib(n) {
  # Llogarit numrin e n-te ne serinë e Fibonaccit ne menyre rekurzive
  nese (n < 2) {
    kthe n;
  }
  
  # Krijimi i variablave lokale brenda funksionit
  shpall rezultat = fib(n - 2) + fib(n - 1);
  kthe rezultat;
}

shpall lista_e_numrave = [1, 5, 8, 10];

per (shpall i = 0; i < lista_e_numrave.gjatesia(); i = i + 1) {
  shpall numri = lista_e_numrave[i];
  printo "Fibonacci per";
  printo numri;
  printo "eshte:";
  printo fib(numri);
  printo "--------------------";
}

# ----------------------------------------------------

printo "--- Testimi i Klasave dhe Objekteve ---";

tip Pije {
  # Nje metode per te pershkruar pijen
  pershkruaj() {
    printo "Une jam nje pije e thjeshte.";
  }
}

tip Kafe < Pije {
  # Nje metode qe mbishkruan metoden e prindit
  pershkruaj() {
    printo "Une jam nje kafe espres e shijshme.";
  }
  
  servir() {
    this.pershkruaj(); # Therrasim metoden tjeter duke perdorur 'this'
    printo "Ju u sherbye!";
  }
}

shpall porosia_ime = Kafe();
porosia_ime.servir();

printo "--------------------";
printo "--- Gjuha DOTAL eshte e kompletuar! ---";