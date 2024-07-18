# PCB

Board size: 45.0x45.0 mm (1.77x1.77 inches)

- This is the size of the rectangle that contains the board
- Thickness: 1.6 mm (63 mils)
- Material: FR4
- Finish: None
- Layers: 2
- Copper thickness: 35 µm

Solder mask: TOP / BOTTOM

- Color: Green

Silk screen: TOP / BOTTOM

- Color: White


Stackup:

| Name                 | Type                 | Color            | Thickness | Material        | Epsilon_r | Loss tangent |
|----------------------|----------------------|------------------|-----------|-----------------|-----------|--------------|
| F.SilkS              | Top Silk Screen      |                  |           |                 |           |              |
| F.Paste              | Top Solder Paste     |                  |           |                 |           |              |
| F.Mask               | Top Solder Mask      |                  |        10 |                 |           |              |
| F.Cu                 | copper               |                  |        35 |                 |           |              |
| dielectric 1         | core                 |                  |      1510 | FR4             |       4.5 |        0.020 |
| B.Cu                 | copper               |                  |        35 |                 |           |              |
| B.Mask               | Bottom Solder Mask   |                  |        10 |                 |           |              |
| B.Paste              | Bottom Solder Paste  |                  |           |                 |           |              |
| B.SilkS              | Bottom Silk Screen   |                  |           |                 |           |              |

# Important sizes

Clearance: 0.2 mm (8 mils)

Track width: 0.2 mm (8 mils)

- By design rules: 0.0 mm (0 mils)

Drill: 0.3 mm (12 mils)

- Vias: 0.3 mm (12 mils) [Design: 0.3 mm (12 mils)]
- Pads: 1.1 mm (43 mils)
- The above values are real drill sizes, they add 0.1 mm (4 mils) to plated holes (PTH)

Via: 0.4/0.2 mm (16/8 mils)

- By design rules: 0.4/0.2 mm (16/8 mils)
- Micro via: yes [0.2/0.1 mm (8/4 mils)]
- Buried/blind via: yes
- Total: 351 (thru: 351 buried/blind: 0 micro: 0)

Outer Annular Ring: 0.05 mm (2 mils)

- By design rules: 0.05 mm (2 mils)

Eurocircuits class: 6F
- Using min drill 0.15 mm for an OAR of 0.13 mm


# General stats

Components count: (SMD/THT)

- Top: 60/2 (SMD + THT)
- Bottom: 0/0 (NONE)

Defined tracks:

- 0.2 mm (8 mils)
- 0.4 mm (16 mils)
- 0.8 mm (31 mils)
- 2.0 mm (79 mils)

Used tracks:

- 0.2 mm (8 mils) (152) defined: yes
- 0.3 mm (12 mils) (4) defined: no
- 0.4 mm (16 mils) (146) defined: yes
- 0.8 mm (31 mils) (1) defined: yes
- 2.0 mm (79 mils) (10) defined: yes
- 2.2 mm (87 mils) (5) defined: no
- 3.0 mm (118 mils) (2) defined: no

Defined vias:

- 0.4/0.2 mm (16/8 mils)
- 0.6/0.3 mm (24/12 mils)

Used vias:

- 0.4/0.2 mm (16/8 mils) (Count: 3, Aspect: 4.0 A) defined: yes
- 0.6/0.3 mm (24/12 mils) (Count: 348, Aspect: 2.7 A) defined: yes

Holes (excluding vias):

- 1.0 mm (39 mils) (4)
- 1.9 mm (75 mils) (5)

Oval holes:


Drill tools (including vias and computing adjusts and rounding):

- 0.3 mm (12 mils) (3)
- 0.4 mm (16 mils) (348)
- 1.1 mm (43 mils) (4)
- 2.0 mm (79 mils) (5)

Solder paste stats:

Using a paste with 87.75 % alloy, that has an specific gravity for the alloy of 7.4 g/cm³
and 1.0 g/cm³ for the flux. This paste has an specific gravity of  4.15 g/cm³.

The stencil thickness is  0.12 mm.

| Side   | Pads with paste | Area [mm²] | Paste [g] |
|--------|-----------------|------------|-----------|
| Total  |             188 |     321.97 |      1.60 |

Note: this is just an approximation to the theoretical value. Margins of the solder mask and waste aren't computed.



