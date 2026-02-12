PUBLIC = 0
ALICE = 1
BOB = 2

config = {
    -- base --
    role = ALICE,
    port = 10240,
    address = '127.0.0.1',

    -- cal --
    expression = 'A+ln(max(A,B))',
    data_path = './data/party_alice.csv',
    save_path = './data/result.csv',
}