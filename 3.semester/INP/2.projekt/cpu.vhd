-- cpu.vhd: Simple 8-bit CPU (BrainLove interphalter)
-- Copyright (C) 2017 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Matus Liscinsky
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='0') / zapis do pameti (DATA_RDWR='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- zde dopiste potrebne deklarace signalu

type instrucion_type is (val_inc, -- typy instrukcii
  val_dec,
  ptr_inc, 
  ptr_dec,
  beg_while, 
  end_while, 
  putc, 
  getc, 
  break, 
  halt, 
  nthin);

signal instr : instrucion_type; -- obsahuje typ akt. spracovavanej instrukcie 
signal set_1  : std_logic; -- signal pre nastavenie programoveho citaca na 1 
signal reg_PC : std_logic_vector(11 downto 0); -- 2^12 = 4096 = velkost ROM
signal PC_inc : std_logic; -- signal pre inkrementaciu prog. citaca
signal PC_dec : std_logic; -- signal pre dekrementaciu prog. citaca

signal reg_PT : std_logic_vector(9 downto 0); -- 2^10 = 1024 = velkost RAM
signal PT_inc : std_logic; -- signal pre inkrementaciu pointra do pamate
signal PT_dec : std_logic; -- signal pre dekrementaciu pointra do pamate

signal reg_CT : std_logic_vector(7 downto 0); -- 2^8 = 256 
signal CT_inc : std_logic; -- signal pre inkrementaciu pocitadla
signal CT_dec : std_logic; -- signal pre dekrementaciu pocitadla

type FSM_state is (state_idle, -- Finite State Machine states 
  s_fetching,
  s_decoding,
  beg_while,
  while_exec,
  while_exec_2,
  while_exec_3,
  end_while,
  end_while_exec,
  end_while_exec_2,
  end_while_exec_3,
  break,
  break2,
  break3,
  nthin,
  ptr_inc,
  ptr_dec,
  val_inc,
  val_inc2,
  val_dec,
  val_dec2,
  putc,
  putc2,
  getc,
  getc2,
  halt );

signal pstate : FSM_state; -- present state
signal nstate : FSM_state; -- next state

signal mux : std_logic_vector(1 downto 0); -- multiplexor pre zapis do pamate

begin

CODE_ADDR <= reg_PC;
DATA_ADDR <= reg_PT;

-- register PC - programovy citac
PC: process (RESET, CLK, reg_PC)
begin
    if (RESET = '1') then
      reg_PC <= (others=>'0');
    elsif (CLK'event) and (CLK = '1') then
      if (PC_inc = '1') then
        reg_PC <= reg_PC + 1;
      elsif (PC_dec = '1') then
        reg_PC <= reg_PC - 1;
      end if;
    end if;
end process;

-- register PT - pre ukladanie adresy aktualnej bunky
PT: process (RESET, CLK, reg_PT)
begin
    if (RESET = '1') then
      reg_PT <= (others=>'0');
    elsif (CLK'event) and (CLK = '1') then
      if (PT_inc = '1') then
        reg_PT <= reg_PT + 1;
      elsif (PT_dec = '1') then
        reg_PT <= reg_PT - 1;
      end if;
    end if;
end process;

-- register CT - ukladanie hodnoty pocitadla (pre while cyklus) 
CT: process(RESET, CLK, reg_CT)
begin
    if (RESET = '1') then
      reg_CT <= (others=>'0');
    elsif (CLK'event) and (CLK = '1') then
      if (CT_inc = '1') then 
        reg_CT <= reg_CT + 1;
      elsif (CT_dec = '1') then
        reg_CT <= reg_CT - 1;
      end if;
    end if;
    -- nastavenie pocitadla pre cykly na 1 
    if( set_1 = '1') then 
    	reg_CT <= "00000001"; 
    end if; 
end process;

-- dekoder instrukcii 
decode_instr: process (CODE_DATA)
begin
  case CODE_DATA is
    when X"3E" => instr <= ptr_inc; -- >
    when X"3C" => instr <= ptr_dec; -- <
    when X"2B" => instr <= val_inc; -- +
    when X"2D" => instr <= val_dec; -- -
    when X"5B" => instr <= beg_while; -- [
    when X"5D" => instr <= end_while; -- ]
    when X"2E" => instr <= putc; -- .
    when X"2C" => instr <= getc; -- ,
    when X"7E" => instr <= break; -- ~
    when X"00" => instr <= halt; -- null
    when others => instr <= nthin; -- comment
    end case;
end process;

-- zmena hodnoty aktualnej bunky
chg_value: process(IN_DATA, DATA_RDATA, mux)
begin
  case mux is 
    when "00" => DATA_WDATA <= IN_DATA;         -- uklada precitane data zo vstupu 
    when "01" => DATA_WDATA <= DATA_RDATA - 1;  -- dekrementacia hodnoty akt. bunky
    when "10" => DATA_WDATA <= DATA_RDATA + 1;  -- inkrementacia hodnoty akt. bunky
    when others =>
  end case;
end process;

-- zmena akt. stavu automatu na nasledujuci
FSM_present_state: process (RESET, CLK)
begin
    if (RESET='1') then
      pstate <= state_idle;
    elsif (CLK'event) and (CLK='1') then
      if (EN = '1') then
        pstate <= nstate;
      end if;
    end if;
end process;

-- NEXT STATE LOGIC
FSM_next_state: process (pstate, instr, DATA_RDATA, IN_VLD, OUT_BUSY, mux)
begin
  -- nastavenie defaultnych hodnot
  PT_dec <= '0';
  PT_inc <= '0';
  PC_inc <= '0';
  PC_dec <= '0';
  CT_inc <= '0';
  CT_dec <= '0';

  CODE_EN <= '0';
  DATA_EN <= '0';
  IN_REQ <= '0';
  OUT_WE <= '0';
  DATA_RDWR <= '0';
  set_1 <= '0'; 
  mux <= "11";

  case(pstate) is 
  
  -- pociatocny stav 
  when state_idle =>
    nstate <= s_fetching;
  
  -- nacitanie instrukcie
  when s_fetching =>
    nstate <= s_decoding;
    CODE_EN <= '1'; 
  
  -- dekodovanie instrukcie 
  when s_decoding =>
    case (instr) is 
      when ptr_inc =>
        nstate <= ptr_inc; 
      when ptr_dec =>
        nstate <= ptr_dec;  
      when val_inc =>
        nstate <= val_inc; 
      when val_dec =>
        nstate <= val_dec; 
      when beg_while =>
        DATA_RDWR <= '0'; -- nastavenie pre citanie z pamate
        DATA_EN <= '1';   
        nstate <= beg_while; 
      when end_while =>   -- nastavenie pre citanie z pamate
        DATA_RDWR <= '0';
        DATA_EN <= '1';  
        nstate <= end_while;
      when putc =>      
        nstate <= putc;   
      when getc =>     
        nstate <= getc; 
      when break =>   
        nstate <= break;
      when halt =>    
        nstate <= halt;
      when nthin => 
        nstate <= nthin;
    end case;

  -- begin while -- [
  when beg_while =>
    PC_inc <= '1';
    nstate <= while_exec;
  
  -- while execute
  when while_exec =>
    -- aka je hodnota aktualnej bunky 0 preskakuje cyklus, medzitym v dalsich stavoch pocita zatvorky [ a ]
    if(DATA_RDATA = "00000000") then
      set_1 <= '1'; 
      nstate <= while_exec_2;
    -- vykonavanie cyklu, nacitanim dalsich instrukcii
    else 
      nstate <= s_fetching;
    end if;

  when while_exec_2 => 
    if(reg_CT = "00000000") then
      nstate <= s_fetching; 
    else
      CODE_EN <= '1';   
      nstate <= while_exec_3;
    end if;

  when while_exec_3 =>
    if( CODE_DATA = X"5B") then -- [
      CT_inc <= '1';
    elsif ( CODE_DATA = X"5D") then -- ]
      CT_dec <= '1';  
    end if;
    nstate <= while_exec_2; -- naspat do predch. stavu "cykli v 2 stavoch"
    PC_inc <= '1';

  -- end while -- ]
  when end_while =>
    -- ak je hodnota akt. bunky 0 nacita dalsiu instrukciu, pokracuje 
    if(DATA_RDATA = "00000000") then
      PC_inc <= '1';
      nstate <= s_fetching;
    -- inak sa vracia az po prislusnu [
    else 
      set_1 <= '1';
      PC_dec <= '1';
      nstate <= end_while_exec;
    end if;

  when end_while_exec =>
    -- ak este nenasiel prislusnu [ pokracuje dalej v cykle
    if(reg_CT /= "00000000") then
      CODE_EN <= '1';
      nstate <= end_while_exec_2;      
    else
    -- nasiel prislusnu [ pokracuje sa nacitanim dalsej instrukcie
      nstate <= s_fetching; 
    end if;

  when end_while_exec_2 =>
    if( CODE_DATA = X"5D") then -- [
      CT_inc <= '1';
    elsif ( CODE_DATA = X"5B") then -- ]
      CT_dec <= '1';
    end if;
    nstate <= end_while_exec_3;
  
  when end_while_exec_3 =>
    if( reg_CT /= "00000000") then
      PC_dec <= '1';
    else 
      PC_inc <= '1';
    end if;
    nstate <= end_while_exec; -- cykli v 3 stavoch 
  
  -- break
  when break =>
    PC_inc <= '1';
    set_1 <= '1';
    nstate <= break2;

  when break2 =>
    -- ak este nenasiel odpovedajucu ] zatvorku 
    if(reg_CT /= "00000000") then
      CODE_EN <= '1';
      nstate <= break3;
    
    -- nasiel,  koniec cyklu, pokracovanie nacitanim dalsej instrukcie
    else
      nstate <= s_fetching;
    end if;
  
  when break3 =>
    if(CODE_DATA = X"5B") then -- [
      CT_inc <= '1';
    elsif (CODE_DATA = X"5D") then -- ]
      CT_dec <= '1';
    end if;
    PC_inc <= '1'; 
    nstate <= break2; -- navrat do prech. stavu (prestavuje cyklus)
  
  -- comment 
  when nthin =>
    PC_inc <= '1';
    nstate <= s_fetching;

  -- inkrementuje pointer a dalej cita instrukcie
  when ptr_inc =>
    PC_inc <= '1';
    PT_inc <= '1';
    nstate <= s_fetching;

  -- dekrementuje pointer a dalej cita instrukcie  
  when ptr_dec => 
    PC_inc <= '1';
    PT_dec <= '1';
    nstate <= s_fetching;
  
  -- inkrementacia hodnoty akt. bunky - 1.faza
  -- nacitanie hodnoty z pamate 
  when val_inc =>
    DATA_EN <= '1';
    DATA_RDWR <= '0';
    nstate <= val_inc2;

  -- inkrementacia hodnoty akt. bunky - 2.faza
  -- zapis inkrementovanej hodnoty bunky do pamate 
  when val_inc2 =>
    DATA_EN <= '1';
    DATA_RDWR <= '1';
    mux <= "10";
    PC_inc <= '1';
    nstate <= s_fetching;

  -- dekrementacia hodnoty akt. bunky - 1.faza
  -- nacitanie hodnoty z pamate
  when val_dec =>
    DATA_EN <= '1';
    DATA_RDWR <= '0';
    nstate <= val_dec2;

  -- dekrementacia hodnoty akt. bunky - 2.faza
  -- zapis dekrementovanej hodnoty bunky do pamate 
  when val_dec2 =>
    DATA_EN <= '1';
    DATA_RDWR <= '1';
    mux <= "01";
    PC_inc <= '1';
    nstate <= s_fetching;

  -- zapis dat na vystup - 1.faza
  -- ak vystup nieje busy, nast. citanie z pamate
  when putc =>
    if(OUT_BUSY = '1')then
    	nstate <= putc;
    else
      DATA_EN <= '1';
      DATA_RDWR <= '0';
      nstate <= putc2;  
    end if;

  -- zapis dat na vystup - 2.faza
  -- samotny zapis a pokracovanie nacitanim dalsej instrukcie
  when putc2 =>
      OUT_DATA <= DATA_RDATA;
      OUT_WE <= '1';
      PC_inc <= '1';
      nstate <= s_fetching;
  
  -- nacitanie hodnoty zo vstupu - 1.faza
  -- ak su data zo vstupu validne pokracuje sa do dalsej fazy 
  when getc =>
    if(IN_VLD = '0') then
      nstate <= getc;
    else
      nstate <= getc2;
    end if;
    IN_REQ <= '1';
  
  -- nacitanie hodnoty zo vstupu - 2.faza
  -- zapis do pamate do akt.bunky a pokracovanie nacitanim dalsej instrukcie 
  when getc2 =>
      DATA_EN <= '1';
      DATA_RDWR <= '1'; 
      mux <= "00";
      PC_inc <= '1';
      nstate <= s_fetching;
  
  -- halt 
  when halt =>
    nstate <= halt;

  end case;
end process;
end behavioral;
 
