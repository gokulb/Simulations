function ana(ds, fileName)
if nargin == 1
		fileName = 'res.txt';
elseif nargin == 0
		dataSize = 300000;
		fileName = 'res.txt';
end
fid = fopen(fileName);
dataSize = ds;
%dataSize = 400000;
%fid = fopen('res2.txt');
join.node = [];
join.time = [];
query.node = [];
query.time = [];
query.file = [];
finish.node = [];
finish.time = [];
finish.file = [];
start.node = [];
start.time = [];

while 1
    tline = fgetl(fid);
    if ~ischar(tline)
	break
    end

    pos = strfind(tline, 'Join Successfully');
    if ~isempty(pos)
	join.node = [ join.node, GetNode(tline)];
	join.time = [ join.time, GetTime(tline)];
    end
    pos = strfind(tline, 'wants to search file with FileTag');
    if ~isempty(pos)
	query.node = [ query.node, GetNode(tline) ];
	query.time = [ query.time, GetTime(tline) ];
	query.file = [ query.file, GetFile(tline) ];
    end
    pos = strfind(tline, 'has finished downloading');
    if ~isempty(pos)
	finish.node = [ finish.node, GetNode(tline) ];
	finish.time = [ finish.time, GetTime(tline) ];
	finish.file = [ finish.file, GetFile(tline) ];
    end
    pos = strfind(tline, 'Connection to tracker has been acked');
    if ~isempty(pos)
	start.node =  [start.node, GetNode(tline)];
	start.time = [start.time, GetTime(tline)];
    end
end
disp(['join.node: ', num2str(join.node)]);
disp(['join.time: ', num2str(join.time)]);
disp(['query.node: ', num2str(query.node)]);
disp(['query.time: ', num2str(query.time)]);
disp(['query.file: ', num2str(query.file)]);
disp(['finish.node: ', num2str(finish.node)]);
disp(['finish.time: ', num2str(finish.time)]);
disp(['finish.file: ', num2str(finish.file)]);
adsVec = [];
delayVec = [];
ufNum = 0;
for i = 1:length(query.time)
    % Average Query Delay
    p0 = find( start.node == query.node(i) );
    if isempty(p0)
	disp(['the query in Node[', num2str(query.node(i)) ,...
		'] for file [', num2str(query.file(i)), '] haven''t started']);
		ufNum = ufNum + 1;
	continue
    end
    delay = start.time(p0) - query.time(i);
    disp(['the avearge querying delay to Node [', ...
	    num2str(query.node(i)),'] for file [', ...
	    num2str(query.file(i)),'] is : ', num2str(delay), 's'])
    delayVec = [delayVec, delay];

    % Average downloading rate
    p = find( (finish.node == query.node(i)) & ...
	    (finish.file == query.file(i)));
    if isempty(p)
	disp(['the query in Node[', num2str(query.node(i)) ,...
		'] for file [', num2str(query.file(i)), '] haven''t finished']);
		ufNum = ufNum + 1;
	continue
    end
    dur = finish.time(p) - start.time(p0);
    aveDownSpeed = dataSize / dur;
	adsVec = [adsVec, aveDownSpeed];
    disp(['the avearge downloading speed to Node [', ...
	    num2str(query.node(i)),'] for file [', ...
	    num2str(query.file(i)),'] is : ', num2str(aveDownSpeed / 1000), 'KBps'])

end
disp(['total average querying delay is ', num2str(mean(delayVec)) ,'s'])
disp(['total average downloading speed is ', num2str(mean(adsVec/1000)) ,'KBps'])
disp(['num of nodes that fail to finish downloading: ', num2str(ufNum)]);
fclose(fid);
