ALTER DATABASE xlisci02 DEFAULT CHARACTER SET utf8;
ALTER DATABASE xlisci02 DEFAULT COLLATE utf8_general_ci;

SET foreign_key_checks = 0;
DROP TABLE IF EXISTS USERS;
DROP TABLE IF EXISTS PERSON;
DROP TABLE IF EXISTS NURSE;
DROP TABLE IF EXISTS VET;
DROP TABLE IF EXISTS `OWNER`;
DROP TABLE IF EXISTS ANIMAL;
DROP TABLE IF EXISTS INTERVENTION;
DROP TABLE IF EXISTS MEDICAMENT;
DROP TABLE IF EXISTS MEDICAMENT_TYPE;
DROP TABLE IF EXISTS TREATMENT;
DROP TABLE IF EXISTS PRESCRIPTION;
DROP TABLE IF EXISTS ANIMAL_SPECIES;
DROP TABLE IF EXISTS SUITABLE;
DROP TABLE IF EXISTS INCLUDES;
SET foreign_key_checks = 1;


CREATE TABLE USERS(
    USER_ID INTEGER NOT NULL AUTO_INCREMENT,
    USERNAME VARCHAR(50) NOT NULL UNIQUE,
    `PASSWORD` VARCHAR(255) NOT NULL,
    MAIL VARCHAR (32) NOT NULL,
    PRIMARY KEY (USER_ID)
) ENGINE=InnoDB;

CREATE TABLE PERSON(
    PERSON_ID INTEGER  NOT NULL AUTO_INCREMENT,
    `NAME` VARCHAR(32) NOT NULL,
    IDENTIFICATION_NUMBER DOUBLE NOT NULL,
    BIRTHDATE DATE NOT NULL,
    ADDRESS VARCHAR(32) NOT NULL,
    SEX CHAR NOT NULL,
	PRIMARY KEY (PERSON_ID)
) ENGINE=InnoDB;

CREATE TABLE NURSE(
    PERSON_ID INTEGER NOT NULL, -- PK a zaroven FK odkazujuci na PERSON_ID
    `DEGREE` VARCHAR (10),
    ACCOUNT_NUMBER VARCHAR (24) NOT NULL,
    HOURLY_WAGE DOUBLE NOT NULL,
    USER_ID INTEGER NOT NULL, -- FK odkazujuci na USER_ID
    PRIMARY KEY (PERSON_ID)
) ENGINE=InnoDB;

CREATE TABLE VET(
    PERSON_ID INTEGER NOT NULL, -- PK a zaroven FK odkazujuci na PERSON_ID
    `DEGREE` VARCHAR (10) NOT NULL,
    ACCOUNT_NUMBER VARCHAR (24) NOT NULL,
    HOURLY_WAGE DOUBLE NOT NULL,
    USER_ID INTEGER NOT NULL, -- FK odkazujuci na USER_ID    
    PRIMARY KEY (PERSON_ID)
) ENGINE=InnoDB;

CREATE TABLE `OWNER`(
    PERSON_ID INTEGER NOT NULL, -- PK a zaroven FK odkazujuci na PERSON_ID
    MAIL VARCHAR (32) NOT NULL,
    PHONE_NUMBER VARCHAR (13) NOT NULL,
    PRIMARY KEY (PERSON_ID)
) ENGINE=InnoDB;

CREATE TABLE ANIMAL(
    ANIMAL_ID INTEGER  NOT NULL AUTO_INCREMENT,
    ANIMAL_SPECIES_ID INTEGER NOT NULL, -- FK typ zvierata
    PERSON_ID INTEGER NOT NULL, -- FK vlastnik zvierata
    ANIM_NAME VARCHAR (32) NOT NULL,
    ANIM_BIRTHDATE DATE NOT NULL,
    LAST_EXAMINATION DATE NOT NULL,
    ANIM_SEX char NOT NULL,
    PRIMARY KEY (ANIMAL_ID)
) ENGINE=InnoDB;

CREATE TABLE INTERVENTION(
    INTERVENTION_ID INTEGER  NOT NULL AUTO_INCREMENT,
    ANIMAL_ID INTEGER NOT NULL, -- FK zviera ktore podstupilo zakrok
    VET_ID INTEGER, -- FK ktory veterinar tento zakrok vykonal
    NURSE_ID INTEGER, -- FK ktora sestricka tento zakrok vykonala
    `TYPE` VARCHAR(32) NOT NULL,
    DOSAGE VARCHAR(32),
    `DATE` DATE NOT NULL,
    SIDE_EFFECTS VARCHAR(50),
    PRICE DOUBLE NOT NULL,
    PRIMARY KEY (INTERVENTION_ID)
) ENGINE=InnoDB;

CREATE TABLE INCLUDES(
    MEDICAMENT_ID INTEGER NOT NULL, -- FK
    PRESCRIPTION_ID INTEGER NOT NULL, -- FK
    DOSAGE VARCHAR(32) NOT NULL,
    PRIMARY KEY (MEDICAMENT_ID, PRESCRIPTION_ID)
) ENGINE=InnoDB;

CREATE TABLE MEDICAMENT(
    MEDICAMENT_ID INTEGER  NOT NULL AUTO_INCREMENT,
    MEDICAMENT_TYPE_ID INTEGER NOT NULL, -- FK typ lieku
    `NAME` VARCHAR(32) NOT NULL,
    SIDE_EFFECTS VARCHAR(50),
    CONTRAINDICATION VARCHAR(50) NOT NULL,
    ACTIVE_SUBSTANCE VARCHAR(50) NOT NULL,
    PRIMARY KEY (MEDICAMENT_ID)
) ENGINE=InnoDB;

CREATE TABLE MEDICAMENT_TYPE(
    MEDICAMENT_TYPE_ID INTEGER  NOT NULL AUTO_INCREMENT,
    ATC_CLASIFICATION VARCHAR(32) NOT NULL,
    MODE_OF_ACTION VARCHAR(32) NOT NULL,
    PRIMARY KEY (MEDICAMENT_TYPE_ID)
) ENGINE=InnoDB;

CREATE TABLE PRESCRIPTION(
    PRESCRIPTION_ID INTEGER  NOT NULL AUTO_INCREMENT,
    TREATMENT_ID INTEGER NOT NULL, -- FK liecba ku ktorej patri recept 
    ADMINISTRATION_TIME VARCHAR(32) NOT NULL,
    `DATE` DATE NOT NULL,
    PRIMARY KEY (PRESCRIPTION_ID)
) ENGINE=InnoDB;

CREATE TABLE TREATMENT(
    TREATMENT_ID INTEGER  NOT NULL AUTO_INCREMENT,
    PERSON_ID INTEGER NOT NULL, -- FK liecba urcena veterinarom VET_ID
    ANIMAL_ID INTEGER NOT NULL, -- FK liecba ktory podstupilo zviera ANIMAL_ID
    DIAGNOSIS VARCHAR(32) NOT NULL,
    DATE_OF_START DATE NOT NULL,
    PRICE DOUBLE NOT NULL,
    PRIMARY KEY (TREATMENT_ID)
 ) ENGINE=InnoDB;
 
CREATE TABLE SUITABLE (
    MEDICAMENT_ID INTEGER NOT NULL, -- fK
    ANIMAL_SPECIES_ID INTEGER NOT NULL, -- fk
    DOSE VARCHAR(32) NOT NULL,
    DISSEASE VARCHAR(128) NOT NULL,
    PRIMARY KEY (MEDICAMENT_ID, ANIMAL_SPECIES_ID)
) ENGINE=InnoDB;

CREATE TABLE ANIMAL_SPECIES(
    ANIMAL_SPECIES_ID INTEGER  NOT NULL AUTO_INCREMENT,
  	SPECIE VARCHAR(32) NOT NULL,
    PRIMARY KEY (ANIMAL_SPECIES_ID)
) ENGINE=InnoDB;


-- FOREIGN KEYS

-- tieto tri entity su v generalizacnom vztahu s entitou person
ALTER TABLE NURSE ADD CONSTRAINT FK_NURSE_ID FOREIGN KEY (PERSON_ID) REFERENCES PERSON(PERSON_ID);
ALTER TABLE VET ADD CONSTRAINT FK_VET_ID FOREIGN KEY (PERSON_ID) REFERENCES PERSON(PERSON_ID);
ALTER TABLE `OWNER` ADD CONSTRAINT FK_PERSON_ID FOREIGN KEY (PERSON_ID) REFERENCES PERSON(PERSON_ID);

-- USERS
ALTER TABLE NURSE ADD CONSTRAINT FK_NURSE_USER_ID FOREIGN KEY (USER_ID) REFERENCES USERS(USER_ID);
ALTER TABLE VET ADD CONSTRAINT FK_VET_USER_ID FOREIGN KEY (USER_ID) REFERENCES USERS(USER_ID);

ALTER TABLE ANIMAL ADD CONSTRAINT FK_OWNER FOREIGN KEY (PERSON_ID) REFERENCES `OWNER`(PERSON_ID); 
ALTER TABLE ANIMAL ADD CONSTRAINT FK_TYPE FOREIGN KEY (ANIMAL_SPECIES_ID) REFERENCES ANIMAL_SPECIES(ANIMAL_SPECIES_ID); 
ALTER TABLE INTERVENTION ADD CONSTRAINT FK_ANIMAL FOREIGN KEY(ANIMAL_ID) REFERENCES ANIMAL(ANIMAL_ID);
ALTER TABLE INTERVENTION ADD CONSTRAINT FK_VET FOREIGN KEY(VET_ID) REFERENCES VET(PERSON_ID);     
ALTER TABLE INTERVENTION ADD CONSTRAINT FK_NURSE FOREIGN KEY(NURSE_ID) REFERENCES NURSE(PERSON_ID); 
ALTER TABLE TREATMENT ADD CONSTRAINT FK_DETERMINED_BY_VET FOREIGN KEY(PERSON_ID) REFERENCES VET(PERSON_ID); 
ALTER TABLE TREATMENT ADD CONSTRAINT FK_UNDERGONE_BY_ANIMAL FOREIGN KEY(ANIMAL_ID) REFERENCES ANIMAL(ANIMAL_ID); 
ALTER TABLE PRESCRIPTION ADD CONSTRAINT FK_TREATMENT FOREIGN KEY(TREATMENT_ID) REFERENCES TREATMENT(TREATMENT_ID); 

ALTER TABLE INCLUDES ADD CONSTRAINT FK_MEDICAMENT FOREIGN KEY(MEDICAMENT_ID) REFERENCES MEDICAMENT(MEDICAMENT_ID); 
ALTER TABLE INCLUDES ADD CONSTRAINT FK_PRESCRIPTION FOREIGN KEY(PRESCRIPTION_ID) REFERENCES PRESCRIPTION(PRESCRIPTION_ID);

ALTER TABLE MEDICAMENT ADD CONSTRAINT FK_MED_TYPE FOREIGN KEY(MEDICAMENT_TYPE_ID) REFERENCES MEDICAMENT_TYPE(MEDICAMENT_TYPE_ID); 
ALTER TABLE SUITABLE ADD CONSTRAINT FK_MEDIC FOREIGN KEY(MEDICAMENT_ID) REFERENCES MEDICAMENT(MEDICAMENT_ID); 
ALTER TABLE SUITABLE ADD CONSTRAINT FK_ANIMAL_SPECIES FOREIGN KEY(ANIMAL_SPECIES_ID) REFERENCES ANIMAL_SPECIES(ANIMAL_SPECIES_ID); 

-- CHECKS
ALTER TABLE VET ADD CONSTRAINT CH_VET_ACCOUNT CHECK ( LENGTH(ACCOUNT_NUMBER) = 24); 
ALTER TABLE NURSE ADD CONSTRAINT CH_NURSE_ACCOUNT CHECK ( LENGTH(ACCOUNT_NUMBER) = 24); 

ALTER TABLE `OWNER` ADD CONSTRAINT CH_PHONE_NUM CHECK ( LENGTH(PHONE_NUMBER) = 13);
ALTER TABLE PERSON ADD CONSTRAINT CHP_SEX CHECK ( SEX = 'M' OR SEX = 'F');

ALTER TABLE ANIMAL ADD CONSTRAINT CHA_SEX CHECK ( ANIM_SEX = 'M' OR ANIM_SEX = 'F');

ALTER TABLE VET ADD CONSTRAINT CH_VET_WAGE CHECK ( HOURLY_WAGE > 0);
ALTER TABLE NURSE ADD CONSTRAINT CH_NURSE_WAGE CHECK ( HOURLY_WAGE > 0);

-- INSERTS


INSERT INTO PERSON (`NAME`, IDENTIFICATION_NUMBER, BIRTHDATE, ADDRESS, SEX) VALUES 
('Jakub Sopka', 9305036840, STR_TO_DATE('1993-05-03', '%Y-%m-%d'), 'Pastovnicka 47 Plavec', 'M'),
('Kristina Svarovska', 9061111532, STR_TO_DATE('1990-11-11', '%Y-%m-%d'), 'Kounicova 114 Presov', 'F'),
('Marian Bazinga', 8907013313, STR_TO_DATE('1989-07-01', '%Y-%m-%d'), 'Jarkova 95 Bardejov', 'M'),
('Svetlana Bielikova', 9155087733, STR_TO_DATE('1991-05-08', '%Y-%m-%d'), 'Na rovni 231 Kosice', 'F'),
('Rudolf Stramak', 8412164860, STR_TO_DATE('1984-12-16', '%Y-%m-%d'), 'Zeleznicna 986 Poprad', 'M'),
('Jaromir Varga', 8104086320, STR_TO_DATE('1981-04-08', '%Y-%m-%d'), 'Kapacka 1178 Trebisov', 'M'),
('Peter Velonos', 9261026588, STR_TO_DATE('1992-11-02', '%Y-%m-%d'), 'Hlavna 114 Lubotice', 'M'),
('Katka Sitniakova', 9708118442, STR_TO_DATE('1997-08-11', '%Y-%m-%d'), 'Somalska 763 Michalovce', 'F');

INSERT INTO USERS(USERNAME, `PASSWORD`, MAIL) VALUES
('vet', 'vet', 'sopka93@gmail.com'),
('varga81', 'srnec48', 'varga81@gmail.com'),
('tinka12', 'lan173', 'tinka12@gmail.com'),
('nurse', 'nurse', 'sveta66@gmail.com'),
('admin', 'admin', 'spse1po@gmail.com');

INSERT INTO VET( PERSON_ID, `DEGREE`, ACCOUNT_NUMBER, HOURLY_WAGE, USER_ID) VALUES 
(1, 'MVDr.', 'SK4083605207004200012345' , 12.5, 1),
(6, 'MVDr.', 'SK6807200002891987426353', 15, 2);

INSERT INTO NURSE( PERSON_ID, `DEGREE`, ACCOUNT_NUMBER, HOURLY_WAGE, USER_ID) VALUES 
(2, 'Mgr.', 'SK6807200004125478965214', 7, 3),
(4, 'Mgr.', 'SK6807200001258885478543', 7.5, 4);


INSERT INTO `OWNER` (PERSON_ID, MAIL, PHONE_NUMBER) VALUES 
(3, 'bazinga@gmail.com', '+420902915778'),
(5, 'stramak123@zoznam.sk', '+421910412786'),
(7, 'vel123peter@centrum.cz', '+420941524896'),
(8, 'katusa777@gmail.com', '+421911236541'),
(1, 'sopka447mvdr@gmail.com', '+421441256541'),
(2, 'mojemail@gmail.com', '+421421451541'),
(4, 'juliana12@gmail.com', '+421421756123'),
(6, 'salalaj@gmail.com', '+421445756541');


INSERT INTO ANIMAL_SPECIES (SPECIE) VALUES 
('Aardvark'),
('Abyssinian'),
('Adelie Penguin'),
('Affenpinscher'),
('African Bush Elephant' ),
('African Palm Civet'),
('African Penguin'),
('African Tree Toad'),
('African Wild Dog'),
('Ainu Dog'),
('Airedale Terrier'),
('Akbash'),
('Akita'),
('Alaskan Malamute'),
('Albatross'),
('Aldabra Giant Tortoise'),
('Alligator'),
('Alpine Dachsbracke'),
('American Bulldog'),
('American Cocker Spaniel'),
('American Coonhound'),
('American Eskimo Dog'),
('American Foxhound'),
('American Pit Bull Terrier'),
('American Staffordshire Terrier'),
('American Water Spaniel'),
('Anatolian Shepherd Dog'),
('Angelfish'),
('Ant'),
('Anteater'),
('Antelope'),
('Appenzeller Dog'),
('Arctic Fox'),
('Arctic Hare'),
('Arctic Wolf'),
('Armadillo'),
('Asian Elephant'),
('Asian Giant Hornet'),
('Asian Palm Civet'),
('Asiatic Black Bear'),
('Australian Cattle Dog'),
('Australian Kelpie Dog'),
('Australian Mist'),
('Australian Shepherd'),
('Australian Terrier'),
('Avocet'),
('Axolotl'),
('Aye Aye'),
('Baboon'),
('Bactrian Camel'),
('Badger'),
('Balinese'),
('Banded Palm Civet'),
('Bandicoot'),
('Barb'),
('Barn Owl'),
('Barnacle'),
('Barracuda'),
('Basenji Dog'),
('Basking Shark'),
('Basset Hound'),
('Bat'),
('Bavarian Mountain Hound'),
('Beagle'),
('Bear'),
('Bearded Collie'),
('Bearded Dragon'),
('Beaver'),
('Bedlington Terrier'),
('Beetle'),
('Bengal Tiger'),
('Bernese Mountain Dog'),
('Bichon Frise'),
('Binturong'),
('Bird'),
('Birds Of Paradise'),
('Birman'),
('Bison'),
('Black Bear'),
('Black Rhinoceros'),
('Black Russian Terrier'),
('Black Widow Spider'),
('Bloodhound'),
('Blue Lacy Dog'),
('Blue Whale'),
('Bluetick Coonhound'),
('Bobcat'),
('Bolognese Dog'),
('Bombay'),
('Bongo'),
('Bonobo'),
('Booby'),
('Border Collie'),
('Border Terrier'),
('Bornean Orang-utan'),
('Borneo Elephant'),
('Boston Terrier'),
('Bottle Nosed Dolphin'),
('Boxer Dog'),
('Boykin Spaniel'),
('Brazilian Terrier'),
('Brown Bear'),
('Budgerigar'),
('Buffalo'),
('Bull Mastiff'),
('Bull Shark'),
('Bull Terrier'),
('Bulldog'),
('Bullfrog'),
('Bumble Bee'),
('Burmese'),
('Burrowing Frog'),
('Butterfly'),
('Butterfly Fish'),
('Caiman'),
('Caiman Lizard'),
('Cairn Terrier'),
('Camel'),
('Canaan Dog'),
('Capybara'),
('Caracal'),
('Carolina Dog'),
('Cassowary'),
('Cat'),
('Caterpillar'),
('Catfish'),
('Cavalier King Charles Spaniel'),
('Centipede'),
('Cesky Fousek'),
('Chameleon'),
('Chamois'),
('Cheetah'),
('Chesapeake Bay Retriever'),
('Chicken'),
('Chihuahua'),
('Chimpanzee'),
('Chinchilla'),
('Chinese Crested Dog'),
('Chinook'),
('Chinstrap Penguin'),
('Chipmunk'),
('Chow Chow'),
('Cichlid'),
('Clouded Leopard'),
('Clown Fish'),
('Clumber Spaniel'),
('Coati'),
('Cockroach'),
('Collared Peccary'),
('Collie'),
('Common Buzzard'),
('Common Frog'),
('Common Loon'),
('Common Toad'),
('Coral'),
('Cottontop Tamarin'),
('Cougar'),
('Cow'),
('Coyote'),
('Crab'),
('Crab-Eating Macaque'),
('Crane'),
('Crested Penguin'),
('Crocodile'),
('Cross River Gorilla'),
('Curly Coated Retriever'),
('Cuscus'),
('Cuttlefish'),
('Dachshund'),
('Dalmatian'),
('Deer'),
('Desert Tortoise'),
('Deutsche Bracke'),
('Dhole'),
('Dingo'),
('Discus'),
('Doberman Pinscher'),
('Dodo'),
('Dog'),
('Dogo Argentino'),
('Dogue De Bordeaux'),
('Dolphin'),
('Donkey'),
('Dormouse'),
('Dragonfly'),
('Drever'),
('Duck'),
('Dugong'),
('Dunker'),
('Dusky Dolphin'),
('Dwarf Crocodile'),
('Eagle'),
('Earwig'),
('Eastern Gorilla'),
('Eastern Lowland Gorilla'),
('Echidna'),
('Edible Frog'),
('Egyptian Mau'),
('Electric Eel'),
('Elephant'),
('Elephant Seal'),
('Elephant Shrew'),
('Emperor Penguin'),
('Emperor Tamarin'),
('Emu'),
('English Cocker Spaniel'),
('English Shepherd'),
('English Springer Spaniel'),
('Entlebucher Mountain Dog'),
('Epagneul Pont Audemer'),
('Eskimo Dog'),
('Estrela Mountain Dog'),
('Falcon'),
('Fennec Fox'),
('Ferret'),
('Field Spaniel'),
('Fin Whale'),
('Finnish Spitz'),
('Fire-Bellied Toad'),
('Fish'),
('Fishing Cat'),
('Flamingo'),
('Flat Coat Retriever'),
('Flounder'),
('Fly'),
('Flying Squirrel'),
('Fossa'),
('Fox'),
('Fox Terrier'),
('French Bulldog'),
('Frigatebird'),
('Frilled Lizard'),
('Frog'),
('Fur Seal'),
('Galapagos Penguin'),
('Galapagos Tortoise'),
('Gar'),
('Gecko'),
('Gentoo Penguin'),
('Geoffroys Tamarin'),
('Gerbil'),
('German Pinscher'),
('German Shepherd'),
('Gharial'),
('Giant African Land Snail'),
('Giant Clam'),
('Giant Panda Bear'),
('Giant Schnauzer'),
('Gibbon'),
('Gila Monster'),
('Giraffe'),
('Glass Lizard'),
('Glow Worm'),
('Goat'),
('Golden Lion Tamarin'),
('Golden Oriole'),
('Golden Retriever'),
('Goose'),
('Gopher'),
('Gorilla'),
('Grasshopper'),
('Great Dane'),
('Great White Shark'),
('Greater Swiss Mountain Dog'),
('Green Bee-Eater'),
('Greenland Dog'),
('Grey Mouse Lemur'),
('Grey Reef Shark'),
('Grey Seal'),
('Greyhound'),
('Grizzly Bear'),
('Grouse'),
('Guinea Fowl'),
('Guinea Pig'),
('Guppy'),
('Hammerhead Shark'),
('Hamster'),
('Hare'),
('Harrier'),
('Havanese'),
('Hedgehog'),
('Hercules Beetle'),
('Hermit Crab'),
('Heron'),
('Highland Cattle'),
('Himalayan'),
('Hippopotamus'),
('Honey Bee'),
('Horn Shark'),
('Horned Frog'),
('Horse'),
('Horseshoe Crab'),
('Howler Monkey'),
('Human'),
('Humboldt Penguin'),
('Hummingbird'),
('Humpback Whale'),
('Hyena'),
('Ibis'),
('Ibizan Hound'),
('Iguana'),
('Impala'),
('Indian Elephant'),
('Indian Palm Squirrel'),
('Indian Rhinoceros'),
('Indian Star Tortoise'),
('Indochinese Tiger'),
('Indri'),
('Insect'),
('Irish Setter'),
('Irish WolfHound'),
('Jack Russel'),
('Jackal'),
('Jaguar'),
('Japanese Chin'),
('Japanese Macaque'),
('Javan Rhinoceros'),
('Javanese'),
('Jellyfish'),
('Kakapo'),
('Kangaroo'),
('Keel Billed Toucan'),
('Killer Whale'),
('King Crab'),
('King Penguin'),
('Kingfisher'),
('Kiwi'),
('Koala'),
('Komodo Dragon'),
('Kudu'),
('Labradoodle'),
('Labrador Retriever'),
('Ladybird'),
('Leaf-Tailed Gecko'),
('Lemming'),
('Lemur'),
('Leopard'),
('Leopard Cat'),
('Leopard Seal'),
('Leopard Tortoise'),
('Liger'),
('Lion'),
('Lionfish'),
('Little Penguin'),
('Lizard'),
('Llama'),
('Lobster'),
('Long-Eared Owl'),
('Lynx'),
('Macaroni Penguin'),
('Macaw'),
('Magellanic Penguin'),
('Magpie'),
('Maine Coon'),
('Malayan Civet'),
('Malayan Tiger'),
('Maltese'),
('Manatee'),
('Mandrill'),
('Manta Ray'),
('Marine Toad'),
('Markhor'),
('Marsh Frog'),
('Masked Palm Civet'),
('Mastiff'),
('Mayfly'),
('Meerkat'),
('Millipede'),
('Minke Whale'),
('Mole'),
('Molly'),
('Mongoose'),
('Mongrel'),
('Monitor Lizard'),
('Monkey'),
('Monte Iberia Eleuth'),
('Moorhen'),
('Moose'),
('Moray Eel'),
('Moth'),
('Mountain Gorilla'),
('Mountain Lion'),
('Mouse'),
('Mule'),
('Neanderthal'),
('Neapolitan Mastiff'),
('Newfoundland'),
('Newt'),
('Nightingale'),
('Norfolk Terrier'),
('Norwegian Forest'),
('Numbat'),
('Nurse Shark'),
('Ocelot'),
('Octopus'),
('Okapi'),
('Old English Sheepdog'),
('Olm'),
('Opossum'),
('Orang-utan'),
('Ostrich'),
('Otter'),
('Oyster'),
('Pademelon'),
('Panther'),
('Parrot'),
('Patas Monkey'),
('Peacock'),
('Pekingese'),
('Pelican'),
('Penguin'),
('Persian'),
('Pheasant'),
('Pied Tamarin'),
('Pig'),
('Pika'),
('Pike'),
('Pink Fairy Armadillo'),
('Piranha'),
('Platypus'),
('Pointer'),
('Poison Dart Frog'),
('Polar Bear'),
('Pond Skater'),
('Poodle'),
('Pool Frog'),
('Porcupine'),
('Possum'),
('Prawn'),
('Proboscis Monkey'),
('Puffer Fish'),
('Puffin'),
('Pug'),
('Puma'),
('Purple Emperor'),
('Puss Moth'),
('Pygmy Hippopotamus'),
('Pygmy Marmoset'),
('Quail'),
('Quetzal'),
('Quokka'),
('Quoll'),
('Rabbit'),
('Raccoon'),
('Raccoon Dog'),
('Radiated Tortoise'),
('Ragdoll'),
('Rat'),
('Rattlesnake'),
('Red Knee Tarantula'),
('Red Panda'),
('Red Wolf'),
('Red-handed Tamarin'),
('Reindeer'),
('Rhinoceros'),
('River Dolphin'),
('River Turtle'),
('Robin'),
('Rock Hyrax'),
('Rockhopper Penguin'),
('Roseate Spoonbill'),
('Rottweiler'),
('Royal Penguin'),
('Russian Blue'),
('Sabre-Toothed Tiger'),
('Saint Bernard'),
('Salamander'),
('Sand Lizard'),
('Saola'),
('Scorpion'),
('Scorpion Fish'),
('Sea Dragon'),
('Sea Lion'),
('Sea Otter'),
('Sea Slug'),
('Sea Squirt'),
('Sea Turtle'),
('Sea Urchin'),
('Seahorse'),
('Seal'),
('Serval'),
('Sheep'),
('Shih Tzu'),
('Shrimp'),
('Siamese'),
('Siamese Fighting Fish'),
('Siberian'),
('Siberian Husky'),
('Siberian Tiger'),
('Silver Dollar'),
('Skunk'),
('Sloth'),
('Slow Worm'),
('Snail'),
('Snake'),
('Snapping Turtle'),
('Snowshoe'),
('Snowy Owl'),
('Somali'),
('South China Tiger'),
('Spadefoot Toad'),
('Sparrow'),
('Spectacled Bear'),
('Sperm Whale'),
('Spider Monkey'),
('Spiny Dogfish'),
('Sponge'),
('Squid'),
('Squirrel'),
('Squirrel Monkey'),
('Sri Lankan Elephant'),
('Staffordshire Bull Terrier'),
('Stag Beetle'),
('Starfish'),
('Stellers Sea Cow'),
('Stick Insect'),
('Stingray'),
('Stoat'),
('Striped Rocket Frog'),
('Sumatran Elephant'),
('Sumatran Orang-utan'),
('Sumatran Rhinoceros'),
('Sumatran Tiger'),
('Sun Bear'),
('Swan'),
('Tang'),
('Tapanuli Orang-utan'),
('Tapir'),
('Tarsier'),
('Tasmanian Devil'),
('Tawny Owl'),
('Termite'),
('Tetra'),
('Thorny Devil'),
('Tibetan Mastiff'),
('Tiffany'),
('Tiger'),
('Tiger Salamander'),
('Tiger Shark'),
('Tortoise'),
('Toucan'),
('Tree Frog'),
('Tropicbird'),
('Tuatara'),
('Turkey'),
('Turkish Angora'),
('Uakari'),
('Uguisu'),
('Umbrellabird'),
('Vampire Bat'),
('Vervet Monkey'),
('Vulture'),
('Wallaby'),
('Walrus'),
('Warthog'),
('Wasp'),
('Water Buffalo'),
('Water Dragon'),
('Water Vole'),
('Weasel'),
('Welsh Corgi'),
('West Highland Terrier'),
('Western Gorilla'),
('Western Lowland Gorilla'),
('Whale Shark'),
('Whippet'),
('White Faced Capuchin'),
('White Rhinoceros'),
('White Tiger'),
('Wild Boar'),
('Wildebeest'),
('Wolf'),
('Wolverine'),
('Wombat'),
('Woodlouse'),
('Woodpecker'),
('Woolly Mammoth'),
('Woolly Monkey'),
('Wrasse'),
('X-Ray Tetra'),
('Yak'),
('Yellow-Eyed Penguin'),
('Yorkshire Terrier'),
('Zebra'),
('Zebra Shark'),
('Zebu'),
('Zonkey'),
('Zorse');

INSERT INTO ANIMAL (ANIMAL_SPECIES_ID, PERSON_ID, ANIM_NAME, ANIM_BIRTHDATE, LAST_EXAMINATION, ANIM_SEX) VALUES 
(241,3,'Jina', STR_TO_DATE('2015-05-11','%Y-%m-%d'), STR_TO_DATE('2018-01-11','%Y-%m-%d'), 'F'),
(148,5,'Rex', STR_TO_DATE('2016-04-02','%Y-%m-%d'), STR_TO_DATE('2018-02-02','%Y-%m-%d'), 'M'),
(245,3,'Fluffy', STR_TO_DATE('2014-07-26','%Y-%m-%d'), STR_TO_DATE('2017-05-01','%Y-%m-%d'), 'M'),
(245,7,'Mandu', STR_TO_DATE('2016-06-14', '%Y-%m-%d'), STR_TO_DATE('2018-03-25','%Y-%m-%d'), 'F'),
(345,8,'Rick', STR_TO_DATE('2017-02-12', '%Y-%m-%d'), STR_TO_DATE('2018-04-02','%Y-%m-%d'), 'M'),
(365,1,'Bobek', STR_TO_DATE('2017-08-13', '%Y-%m-%d'), STR_TO_DATE('2017-09-22','%Y-%m-%d'), 'M'),

(154,2,'Chevy', STR_TO_DATE('2014-01-15','%Y-%m-%d'), STR_TO_DATE('2018-01-04','%Y-%m-%d'), 'F'),
(12,4,'Wolfie', STR_TO_DATE('2016-04-30','%Y-%m-%d'), STR_TO_DATE('2018-11-05','%Y-%m-%d'), 'M'),
(502,6,'Tink', STR_TO_DATE('2014-07-02','%Y-%m-%d'), STR_TO_DATE('2017-12-01','%Y-%m-%d'), 'M'),
(447,7,'Quattro', STR_TO_DATE('2014-06-24', '%Y-%m-%d'), STR_TO_DATE('2018-03-22','%Y-%m-%d'), 'F'),
(103,8,'Nanda', STR_TO_DATE('2017-02-03', '%Y-%m-%d'), STR_TO_DATE('2018-10-02','%Y-%m-%d'), 'M'),
(79,1,'Nico', STR_TO_DATE('2014-08-11', '%Y-%m-%d'), STR_TO_DATE('2017-06-18','%Y-%m-%d'), 'M'),

(112,3,'Merry', STR_TO_DATE('2015-07-15','%Y-%m-%d'), STR_TO_DATE('2017-01-13','%Y-%m-%d'), 'F'),
(162,5,'Mitzi', STR_TO_DATE('2014-04-02','%Y-%m-%d'), STR_TO_DATE('2017-11-09','%Y-%m-%d'), 'M'),
(333,3,'Redford', STR_TO_DATE('2015-07-02','%Y-%m-%d'), STR_TO_DATE('2017-12-04','%Y-%m-%d'), 'M'),
(198,2,'Quattro', STR_TO_DATE('2015-06-01', '%Y-%m-%d'), STR_TO_DATE('2017-03-22','%Y-%m-%d'), 'F'),
(279,4,'Lefty', STR_TO_DATE('2017-07-28', '%Y-%m-%d'), STR_TO_DATE('2017-10-22','%Y-%m-%d'), 'M'),
(311,6,'Jenna', STR_TO_DATE('2015-08-21', '%Y-%m-%d'), STR_TO_DATE('2017-06-18','%Y-%m-%d'), 'M');

INSERT INTO TREATMENT( PERSON_ID, ANIMAL_ID, DIAGNOSIS, DATE_OF_START, PRICE) VALUES 
(1, 2, 'Knuckleache', STR_TO_DATE('2018-01-28', '%Y-%m-%d'), 17.45 ),
(6, 4, 'Salmonellosis', STR_TO_DATE('2017-02-11', '%Y-%m-%d'), 25 ),
(6, 3, 'Listeriosis', STR_TO_DATE('2018-03-17', '%Y-%m-%d'), 9.50),
(1, 5, 'Fowl cholera', STR_TO_DATE('2018-03-05', '%Y-%m-%d'), 74.50),
(1, 6, 'Ringworm', STR_TO_DATE('2018-09-15', '%Y-%m-%d'), 74.90),
(1, 6, 'Toxocariasis', STR_TO_DATE('2018-07-15', '%Y-%m-%d'), 9.90),

(6, 1, 'Knuckleache', STR_TO_DATE('2018-01-28', '%Y-%m-%d'), 17.45 ),
(1, 2, 'Clostridial disease', STR_TO_DATE('2017-02-11', '%Y-%m-%d'), 25 ),
(1, 3, 'Salmonellosis', STR_TO_DATE('2018-03-17', '%Y-%m-%d'), 9.50),
(6, 4, 'Ringworm', STR_TO_DATE('2018-03-05', '%Y-%m-%d'), 74.50),
(6, 5, 'Listeriosis', STR_TO_DATE('2018-09-15', '%Y-%m-%d'), 74.90),
(1, 6, 'Cattle ticks', STR_TO_DATE('2018-07-15', '%Y-%m-%d'), 9.90),

(6, 7, 'Knuckleache', STR_TO_DATE('2018-01-28', '%Y-%m-%d'), 17.45 ),
(1, 8, 'Aflatoxicosis', STR_TO_DATE('2017-02-11', '%Y-%m-%d'), 25 ),
(1, 9, 'Aflatoxicosis', STR_TO_DATE('2018-03-17', '%Y-%m-%d'), 9.50),
(6, 10, 'Ringworm', STR_TO_DATE('2018-03-05', '%Y-%m-%d'), 74.50),
(6, 11, 'Toxocariasis', STR_TO_DATE('2018-09-15', '%Y-%m-%d'), 74.90),
(1, 12, 'Cattle ticks', STR_TO_DATE('2018-07-15', '%Y-%m-%d'), 9.90),

(6, 13, 'Knuckleache', STR_TO_DATE('2018-01-28', '%Y-%m-%d'), 17.45 ),
(1, 14, 'Clostridial disease', STR_TO_DATE('2017-02-11', '%Y-%m-%d'), 25 ),
(1, 15, 'Aflatoxicosis', STR_TO_DATE('2018-03-17', '%Y-%m-%d'), 9.50),
(6, 16, 'Salmonellosis', STR_TO_DATE('2018-03-05', '%Y-%m-%d'), 74.50),
(6, 17, 'Listeriosis', STR_TO_DATE('2018-09-15', '%Y-%m-%d'), 74.90),
(1, 18, 'Toxocariasis', STR_TO_DATE('2018-07-15', '%Y-%m-%d'), 9.90);

INSERT INTO INTERVENTION(ANIMAL_ID, VET_ID, `TYPE`, `DATE`, PRICE) VALUES 
(1, 6, 'EKG', STR_TO_DATE('2018-02-19', '%Y-%m-%d'), 5.50),
(2, 6, 'EKG', STR_TO_DATE('2018-01-23', '%Y-%m-%d'), 6.50),
(15, 1, 'EKG', STR_TO_DATE('2018-04-07', '%Y-%m-%d'), 7.50),
(14, 1, 'EKG', STR_TO_DATE('2018-03-03', '%Y-%m-%d'), 5.50);

INSERT INTO INTERVENTION(ANIMAL_ID, NURSE_ID, `TYPE`, DOSAGE, `DATE`, SIDE_EFFECTS, PRICE) VALUES 
(3, 4, 'Blood collection and sample processing', '50ml', STR_TO_DATE('2018-01-23', '%Y-%m-%d'),'Dizziness',2.60),
(4, 4, 'Blood collection and sample processing', '25ml', STR_TO_DATE('2017-08-02', '%Y-%m-%d'),'Dizziness',1.60),
(13, 2, 'Blood collection and sample processing', '50ml', STR_TO_DATE('2018-07-03', '%Y-%m-%d'),'Dizziness',2.30),
(12, 2, 'Blood collection and sample processing', '50ml', STR_TO_DATE('2018-10-14', '%Y-%m-%d'),'Dizziness',2.40);

INSERT INTO INTERVENTION(ANIMAL_ID, NURSE_ID, `TYPE`, DOSAGE, `DATE`, PRICE) VALUES 
(5, 2, 'Injection anesthesia', '25ml', STR_TO_DATE('2018-01-14', '%Y-%m-%d'),18.90),
(6, 4, 'Inhalation anesthesia', '25ml', STR_TO_DATE('2018-08-22', '%Y-%m-%d'),15.90),
(11, 2, 'Injection anesthesia', '25ml', STR_TO_DATE('2018-09-28', '%Y-%m-%d'),15.90),
(10, 4, 'Inhalation anesthesia', '25ml', STR_TO_DATE('2018-10-05', '%Y-%m-%d'),18.90);

INSERT INTO INTERVENTION(ANIMAL_ID, VET_ID, NURSE_ID, `TYPE`, `DATE`, PRICE) VALUES 
(9, 1, 2, 'Puncture of the urinary bladder', STR_TO_DATE('2017-08-02', '%Y-%m-%d'),60.20),
(8, 1, 4, 'Leg amputation', STR_TO_DATE('2017-09-12', '%Y-%m-%d'),50.90),
(17, 6, 2, 'Binding the wound', STR_TO_DATE('2017-10-18', '%Y-%m-%d'),20.00),
(16, 6, 4, 'Binding the wound', STR_TO_DATE('2017-09-19', '%Y-%m-%d'),31.00),
(18, 6, 2, 'Puncture of the urinary bladder', STR_TO_DATE('2017-08-02', '%Y-%m-%d'),60.20),
(7, 1, 4, 'Binding the wound', STR_TO_DATE('2017-09-19', '%Y-%m-%d'),35.00);



INSERT INTO PRESCRIPTION( TREATMENT_ID, ADMINISTRATION_TIME, `DATE`) VALUES 
(1,'2 weeks',  STR_TO_DATE('2018-01-28', '%Y-%m-%d')),
(2,'1 week', STR_TO_DATE('2017-02-11', '%Y-%m-%d')),
(3,'1 week', STR_TO_DATE('2018-03-17', '%Y-%m-%d')),
(4,'1 day', STR_TO_DATE('2016-03-05', '%Y-%m-%d')),

(5,'2 weeks',  STR_TO_DATE('2018-01-28', '%Y-%m-%d')),  
(6,'3 weeks', STR_TO_DATE('2017-02-11', '%Y-%m-%d')),
(7,'4 weeks', STR_TO_DATE('2018-03-17', '%Y-%m-%d')),
(8,'14 days', STR_TO_DATE('2016-03-05', '%Y-%m-%d')),

(9,'6 weeks',  STR_TO_DATE('2018-01-28', '%Y-%m-%d')),
(10,'5 weeks', STR_TO_DATE('2017-02-11', '%Y-%m-%d')),
(11,'2 weeks', STR_TO_DATE('2018-03-17', '%Y-%m-%d')),
(12,'12 days', STR_TO_DATE('2016-03-05', '%Y-%m-%d')),

(13,'6 weeks',  STR_TO_DATE('2018-01-28', '%Y-%m-%d')),
(14,'5 weeks', STR_TO_DATE('2017-02-11', '%Y-%m-%d')),
(15,'2 weeks', STR_TO_DATE('2018-03-17', '%Y-%m-%d')),
(16,'12 days', STR_TO_DATE('2016-03-05', '%Y-%m-%d')),

(17,'6 weeks',  STR_TO_DATE('2018-01-28', '%Y-%m-%d')),
(18,'5 weeks', STR_TO_DATE('2017-02-11', '%Y-%m-%d')),
(19,'2 weeks', STR_TO_DATE('2018-03-17', '%Y-%m-%d')),
(20,'12 days', STR_TO_DATE('2016-03-05', '%Y-%m-%d')),

(21,'2 weeks',  STR_TO_DATE('2018-01-28', '%Y-%m-%d')),
(22,'4 weeks', STR_TO_DATE('2017-02-11', '%Y-%m-%d')),
(23,'3 weeks', STR_TO_DATE('2018-03-17', '%Y-%m-%d')),
(24,'11 days', STR_TO_DATE('2016-03-05', '%Y-%m-%d'));

INSERT INTO MEDICAMENT_TYPE( ATC_CLASIFICATION, MODE_OF_ACTION ) VALUES 
('Analgesic', 'Pain relief'),
('Antibiotic', 'Treatment of infectious diseases'),
('Antiparasitic', 'Against the parasite');

INSERT INTO MEDICAMENT( MEDICAMENT_TYPE_ID, `NAME`, CONTRAINDICATION, ACTIVE_SUBSTANCE, SIDE_EFFECTS ) VALUES 
(1, 'Rycarfa Flavour', 'Do not use in pregnant and lactating female dog', 'Carprofenum', 'Vomiting, faeces, loss of appetite'),
(2, 'Quiflox', 'Do not use in dogs younger than 12 months', 'Marbofloxacinum', 'Pain and the appearance of igneous lesions'),
(2, 'Enroxil Flavour', 'Do not use as prophylaxis', 'Enrofloxacin', 'Vomiting and anorexia'),
(1, 'Ataxxa', 'Do not apply to younger than 7 weeks old', 'Permathrinum', 'Drowsiness, triage, edema, erosion');

INSERT INTO SUITABLE( MEDICAMENT_ID, ANIMAL_SPECIES_ID, DOSE, DISSEASE) VALUES 
(1,1,'2-4mg/kg','relieving inflammation and joint pain'),
(2,1,'2mg/kg','treatment of infected wounds, infections of lower power routes'),
(2,4,'2mg/kg','treatment of respiratory infections'),
(3,1,'5mg/kg','treatment of bacterial infections of the gastrointestinal, respiratory and urogenital tract'),
(3,2,'5mg/kg','treatment of bacterial infections of the gastrointestinal, respiratory and urogenital tract,'),
(4,3,'10mg/kg','treatment of blisters');

INSERT INTO INCLUDES( MEDICAMENT_ID, PRESCRIPTION_ID, DOSAGE ) VALUES
( 1,1,'1dose/8hod' ),
( 3,2,'1dose/12hod' ),
( 3,3,'1dose/8hod'),
( 4,4,'1dose' ),

( 1,5,'1dose/8hod' ),
( 3,6,'1dose/12hod' ),
( 3,7,'1dose/8hod'),
( 4,24,'1dose' ),

( 1,8,'1dose/8hod' ),
( 3,9,'1dose/12hod' ),
( 3,10,'1dose/8hod'),
( 4,11,'1dose' ),

( 1,12,'1dose/8hod' ),
( 3,13,'1dose/12hod' ),
( 3,14,'1dose/8hod'),
( 4,15,'1dose' ),

( 1,16,'1dose/8hod' ),
( 3,17,'1dose/12hod' ),
( 3,18,'1dose/8hod'),
( 4,19,'1dose' ),

( 1,20,'1dose/8hod' ),
( 3,21,'1dose/12hod' ),
( 3,22,'1dose/8hod'),
( 4,23,'1dose' );