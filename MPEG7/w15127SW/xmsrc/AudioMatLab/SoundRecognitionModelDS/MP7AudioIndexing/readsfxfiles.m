function sfxfiles = readsfxfiles(sfxfilename)

% READSFXFILES - read a text file into a cell array of file names  
%                                                                  
%     sfxfiles = readsfxfiles(icfilename)                          
% ...


if(1)
    sfxfiles=textread(sfxfilename,'%s','delimiter','\n','whitespace','');
else
    fid = fopen(sfxfilename, 'rt');
    if fid<=0
	error(['Unable to open ' sfxfilename ' as a text file']);
    end

    sfxfiles = cell(1);
    f = 1;


    % One filename per line
    sfxfile = fgetl(fid);
    while ~isempty(sfxfile) & (sfxfile ~= -1)
	sfxfiles{f} = sfxfile;
	f=f+1;
	sfxfile = fgetl(fid);
    end
    fclose(fid);

    sfxfiles=sfxfiles';
end
