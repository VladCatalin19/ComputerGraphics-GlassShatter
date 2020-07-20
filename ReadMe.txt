	1) Descriere
	Pentru această temă am creat o scenă similară cu o cameră. În această cameră
se află mai multe obiecte decorative și o fereastră. Prin apăsarea click stânga
se aruncă o bilă pe direcția camerei. Această bilă interacționează doar cu gea-
mul, pe care îl poate sparge. După coliziunea dintre bilă și geam, sticla se
sparge, iar cioburile cad.
	Timpul se poate dilata / contracta. Pe apăsarea tastei „-” timpul înceti-
nește, „+” timpul accelerează, „[” timpul se oprește, iar pe „]” timpul revine
la normal. De asemenea, geamul și cioburile pot fi afișate în mod reflexie-re-
fracție folosind tasta „1”, în mod mod mapare normală folosind tasta „2” și în
mod wireframe folosind tasta „2”.

	2) Implementare
	Pentru generarea cioburilor am folosit diagrame voronoi. Sunt generate mai
multe puncte pe 3 cercuri concentrice, cu centrul punctul de impact. Acestea sunt
trimise API-ului care generează apoi punctele pentru fiecare poligon din diagrama
voronoi. Punctele sunt trimise către GPU folosind un SSBO, iar în Geometry Shader
din puncte care delimitează un poligon, se transformă într-o prismă cu baza acel
poligon. Tot pe GPU se calculează poziția și rotația fiecărui ciob.
	Pentru randarea în cubemap, am genereat un fbo, un depth buffer și o textură
de tip cubemap goală. La fiecare cadru, fac bind la fiecare față a cubemap-ului,
mut camera și randez scena. După randarea întregului cubemap, îl trimit către GPU
pentru a fi folosit în calcularea reflexiei și refracției geamului.
	
	3) Resurse
diagrame voronoi - https://github.com/JCash/voronoi
tutorial cubemap - https://www.youtube.com/watch?v=lW_iqrtJORc
obiecte cameră - https://www.turbosquid.com/FullPreview/Index.cfm/ID/764544
laptop - https://free3d.com/3d-model/-laptop-computer-v1--663202.html

	Texturi
lemn - http://bgfons.com/download/3166
lemn podea - http://bgfons.com/download/3803
lemn usă - https://image.freepik.com/free-vector/wooden-white-texture-background_108964-425.jpg
metal - http://www.psd-dude.com/tutorials/resources/metal-and-metallic-textures-for-photoshop.aspx
tartan - https://images.fineartamerica.com/images/artworkimages/mediumlarge/1/color-tartan-texture-hamik-arts.jpg
cearșaf - https://t4.rbxcdn.com/e3256af11bee0ff386bb87817bde20a5
pernă - https://www.istockphoto.com/vector/embroidery-floral-seamless-pattern-on-linen-cloth-texture-gm1073691046-287430662
lavabil - http://beyondmarketinginc.co/wp-content/uploads/2019/02/seamless-painted-wall-texture-hr-full-resolution-preview-demo-textures-architecture-plaster-painted-plaster-fine-plaster-painted-wall-texture-seamless-kids-room-curtains-kids-rooms-to-go.jpg
perdele - https://f1af951e8abcbc4c70b9-9997fa854afcb64e87870c0f4e867f1d.ssl.cf3.rackcdn.com/1000004826501-1000004826500_04-710.jpg
margine geam - https://1.bp.blogspot.com/-O0v92JVTNwc/XVhffW1zCII/AAAAAAAAMCM/UIFhOtKQJq0NYswx1jM8YndkPM7a2UiVACEwYBhgL/s1600/paper_seamless_dirty_white_texture2.jpg
tavan - http://moldpres.co/wp-content/uploads/2019/06/white-walls-texture-wall-roll-smooth-decorative-finish-2-house.jpg
poster 1 - https://wallpapercave.com/wp/wp4051808.jpg
poster 2 - https://pics.alphacoders.com/pictures/view/320425
poster 3 - https://pics.alphacoders.com/pictures/view/142002
