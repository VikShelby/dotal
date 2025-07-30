# Testimi i nese/perndryshe per VM

variabel mosha = 25;
variabel ka_leje = vertete;

nese (mosha > 18) {
  shkruaj 1; # 1 do te thote "Mund te votoni"
}

nese (ka_leje == genjeshter) {
  shkruaj 0; # 0 do te thote "Nuk duhet te ndodhe"
} perndryshe {
  shkruaj 2; # 2 do te thote "Mund ti jepni makines"
}

nese (10 > 20) {
    shkruaj -1; # -1 do te thote "Nuk duhet te printohet"
}