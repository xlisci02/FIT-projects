-- 1. Projekt INP
-- Rizeni maticoveho displeje z FPGA
-- Matus Liscinsky
-- xlisci02
-- 4.11.2017

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
    port (
            RESET :  in std_logic;
            SMCLK :  in std_logic;
            ROW :  out std_logic_vector(7 downto 0);
            LED :  out std_logic_vector(7 downto 0)
    );
end ledc8x8;

architecture main of ledc8x8 is
    signal count_1  :  std_logic_vector(7 downto 0) := (others => '0'); -- pomocne pocitadlo pre delic frekvencie 
    signal clock_en :  std_logic := '0'; -- clock enable

    signal leds :  std_logic_vector(7 downto 0) := (others => '1'); -- vektor pre stlpce
    signal rows :  std_logic_vector(7 downto 0) := (0 => '1', others => '0'); -- vektor pre riadky

begin

    -- delic frekvencie
    freq: process (RESET, SMCLK, count_1)
    begin

        if (RESET = '1') then --RESET
            count_1 <= (others => '0');
        elsif (SMCLK'event) and (SMCLK = '1') then
            count_1 <= count_1 + 1; -- ikrementacia pocitadla, reaguje na nastupnu hranu SMCLK
        end if;

        if count_1 = "11111111" then -- pocitadlo dosiahlo max. hodnotu : 255 -> CE=1, clock enabled 
            clock_en <= '1';
        else 
            clock_en <= '0'; -- inak CE=0, 
        end if;

    end process freq;

    -- rotacia "bitov" vektora rows, pre postupny prechod riadkami mat.displeja
    rot_rows: process (RESET, SMCLK, rows, clock_en)
    begin

        if (RESET = '1') then -- RESET
            rows <= (0 => '1', others => '0'); 
        elsif (clock_en = '1') and (SMCLK'event) and (SMCLK = '1') then  
		rows <=  rows(6 downto 0) & rows(7); -- konkatenacia, posun o riadok vramci mat.displeja 
        end if;

    end process rot_rows;

    lights: process(leds, rows)
    begin
	case rows is 
            when "00000001" => leds <= "11101110"; -- case pre kazdy riadok mat.displeja
            when "00000010" => leds <= "11100100";
            when "00000100" => leds <= "11101010";
            when "00001000" => leds <= "11101110";
            when "00010000" => leds <= "11101110";
            when "00100000" => leds <= "11101111";
            when "01000000" => leds <= "11101111";
            when "10000000" => leds <= "00001111";
            when others => leds <= "11111111"; 
        end case;
      
    	LED <= leds; -- prepis vektora LED 
    	ROW <= rows; -- prepis vektora ROW
    
    end process lights;
     
end main;
