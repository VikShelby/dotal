shpall koha_fillestare = koha();

printo "Si quheni?";
shpall emri = lexo();
printo "Përshendetje, " + emri + "!";

shpall koha_perfundimtare = koha();

shpall koha_e_pergjigjes = koha_perfundimtare - koha_fillestare;
printo "U deshen kaq sekonda per t'u pergjigjur:";
printo koha_e_pergjigjes;