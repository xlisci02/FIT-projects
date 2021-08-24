ctmc

const double ki;
const double kr;


module cov20

   Zdravi: [0..95] init 95;

   Nakazeni: [0..100] init 5;

   Uzdraveni: [0..100] init 0;

   [] (Zdravi > 0 & Nakazeni > 0 & Nakazeni < 100) -> Zdravi*Nakazeni*ki: (Nakazeni' = Nakazeni+1) & (Zdravi' = Zdravi-1);

   [] (Nakazeni > 0 & Uzdraveni < 100) -> Nakazeni*kr: (Nakazeni' = Nakazeni-1) & (Uzdraveni' = Uzdraveni+1);

endmodule

