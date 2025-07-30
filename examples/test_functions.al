funksion pershendetje(emer) {
  # NUK MUND TE PERDORIM STRING CONCATENATION AKOMA
  shkruaj "Pershendetje, ";
  shkruaj emer;
}

funksion mblidh(a, b) {
  kthe a + b;
}

shkruaj "Duke thirrur funksionet...";

pershendetje("Gjergj");
variabel shuma = mblidh(100, 23);
shkruaj "100 + 23 eshte:";
shkruaj shuma;

shkruaj mblidh(shuma, shuma);