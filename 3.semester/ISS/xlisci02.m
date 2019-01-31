filename = 'xlisci02.wav';
[s, Fs] = audioread(filename);

% 1.uloha
Fs
length(s)
length(s) / Fs % nasobenim Fs dostaneme Hz = sekunda^-1, delenim dostaneme dlzku v sekundach

% 2.uloha
% Pouzita studijna etapa
N = length(s);
G = 10 * log10(1/N * abs(fft(s)).^2);
f = (0:N/2-1)/N * Fs; G = G(1:N/2);
plot (f,G); 
xlabel('f [Hz]'); 
ylabel ('PSD [dB]'); grid; 


% 3.uloha
% https://www.mathworks.com/help/matlab/ref/max.html
[~, i] = max(G);
i = i-1 % indexovanie v matlabe je od 1

% 4.uloha
% Pouzita studijna etapa
b = [ 0.2324 -0.4112 0.2324 ];
a = [ 1 0.2289 0.4662 ];
zplane (b,a); % zero-pole plot
p = roots(a); % korene polynomu v menovateli = poly
if (isempty(p) | abs(p) < 1) 
  disp('STABILNY !')
else
  disp('NESTABILNY !')
end

% 5.uloha

H = freqz(b,a,Fs/2); 
f = (0:N/2-1)/N * Fs; % stare, iba zopakovane
plot (f,abs(H)); grid; xlabel('f'); ylabel('|H(f)|')

% 6.uloha
filtered_signal = filter (b,a,s);
% to iste co v 2.ulohe ale s filtrovanym signalom
G = 10 * log10(1/N * abs(fft(filtered_signal)).^2);
f = (0:N/2-1)/N * Fs; G = G(1:N/2);
plot (f,G); 
xlabel('f [Hz]'); 
ylabel ('PSD [dB]'); grid; 

% 7.uloha
[~, i] = max(G);
i = i-1 % indexovanie v matlabe je od 1

% 8.uloha
% obdlznikovy signal
rectangle = [2 2 -2 -2 ];
% vytvorenie sekvencie - obdoba repmat
for j = 1 : (80*4)
    rectangle(j) = rectangle(mod(j-1,4)+1);
end ;

c = zeros(1,320); % vektor pre vystrihnutu cast zo signalu, postupne sa posuva 
r = zeros(1,320); % vektor pre vysledok nasobenia 
d = zeros(1,16000-319); % vektor pre ukladanie suctov
for index = 1 : 16000-319
    c = s(index:index+319, 1); % vystrihnuta cast z nacitaneho signalu
    c = c'; % zmena na stlpcovy vektor
    r = rectangle .* c; % nasobenie vektorov
    d(index) = abs(sum(r)); % ukladanie suctu nasobkov
end
[~, i] = max (d); % hladanie maxima
i % cislo vzorku
i / Fs % v sekundach

% 9.uloha
k = -50:50; % vektor od -50 do 50
Rv = xcorr(s,'biased'); 
plot(k, Rv(N-50:N+50)); % N je index 0 
grid; 

% 10.uloha
fprintf('%.6f\n',Rv(N+10)); % N je index 0

% 11.uloha
% Pouzita studijna etapa
% https://www.fit.vutbr.cz/study/courses/ISS/public/proj_studijni_etapa/5_random/5_random.pdf
minimum = min(s); maximum = max(s); % min a max signalu
x = linspace(minimum,maximum,60); % linearny vektor od min do max, pocet hodnot 60

% studijna etapa
L = length(x);
N = length(s);
h = zeros(L,L);
xcol = x(:); 
bigx = repmat(xcol,1,N); 
yr = s(:)'; 
bigy = repmat(yr,L,1);
[~,ind1] = min(abs(bigy - bigx)); 
ind2 = ind1(11:N);

for ii=1:N-10,
  d1 = ind1(ii);   d2 = ind2(ii); 
  h(d1,d2) = h(d1,d2) + 1; 
end

surf = (x(2) - x(1))^2; % surface of one tile
p = h / N / surf;  
bar3(p); % vykreslenie

% 12.uloha
check = sum(sum (p)) * surf; % integral, mal by sa blizit ku 1
check 

% 13.uloha
x = x(:); X1 = repmat(x,1,L); % zmena na stlpcovy vektor, rozsirenie na L rovnakych stlpcov 
x = x'; X2 = repmat(x,L,1); % zmena na riadkovy vektor, rozsirenie na L rovnakych riadkov
r10 = sum(sum (X1 .* X2 .* p)) * surf; % vypocet R[10]
fprintf('%.6f\n',r10);

