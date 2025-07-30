# Testimi final: Trashegimia

tip Ushqim {
  # Te gjitha ushqimet kane nje emer
  vendosEmrin(emri_ri) {
    this.emri = emri_ri;
  }

  shfaqEmrin() {
    printo this.emri;
  }
}

# Embelsira eshte nje tip Ushqimi
tip Embelsire < Ushqim {
  # Ajo ka nje metode te vetme
  shijo() {
    printo "Mmm, e shijshme!";
  }
}

shpall bakllava = Embelsire();

# Therrasim nje metode nga klasa femije (Embelsire)
bakllava.shijo();

# Therrasim nje metode te trasheguar nga klasa prind (Ushqim)
bakllava.vendosEmrin("Bakllava e shijshme");
bakllava.shfaqEmrin();