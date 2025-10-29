#
# xbox360iso.py - Xbox 360 ISO / Xex Analysis & Extraction
#
# (c) 2014 Rob Lambell <rob[at]lambell.info>
# This code is licensed under MIT license (see LICENSE for details)
#
# REFERENCE IMPLEMENTATION - Preserved for Velocity-Next ISO/XEX feature development
# See: specs/003-iso-xex-support/spec.md
#

import binascii
import csv
import io
import os.path
from struct import unpack
import sys
import urllib.request


class Xbox360ISO(object):
    """
    Parse an Xbox 360 ISO image and Xex file.
    Related source code:
    * abgx360.c of abgx360
    * http://abgx360.net
    """

    def __init__(self):
        # root offset for types of xbox media
        self.iso_type = {'GDF': 0xfd90000,
                         'XGD3': 0x2080000,
                         'XSF': 0}

        self.csv_file = None
        self.csv_settings = {'local': 'GameNameLookup.csv',
                             'url': 'http://abgx360.net/Apps/Stealth360/GameNameLookup.csv',
                             'force_update': False,
                             'download_if_missing': True,
                             'update_if_no_match': True,
                             'min_age': 60 * 60 * 24}

    def parse(self, filename):
        # open iso
        iso_file = open(filename, "rb")

        # check iso is an xbox 360 game and record some details
        iso_info = self.check_iso(iso_file)
        if iso_info is False:
            iso_file.close()
            return False

        # find and extract default.xex from the iso
        xex_buffer = self.extract_defaultxex(iso_file, iso_info)
        if xex_buffer is False:
            iso_file.close()
            return False
        else:
            iso_info['defaultxex'] = xex_buffer

        # extract game details from default.xex
        xex_info = self.extract_xex_info(xex_buffer)
        if xex_info is False:
            iso_file.close()
            return False

        # lookup the full game name
        xex_info['game_name'] = self.media_id_to_game_name(xex_info['media_id'])

        props = iso_info.copy()
        props.update(xex_info)

        iso_file.close()
        return props

    def check_iso(self, iso_file):
        iso_info = {}

        iso_info['sector_size'] = 0x800
        iso_file.seek((0x20 * iso_info['sector_size']))
        if iso_file.read(20).decode("ascii", "ignore") == 'MICROSOFT*XBOX*MEDIA':
            iso_info['root_offset'] = self.iso_type['XSF']
            print('Original Xbox ISO format not supported')
            return False
        else:
            iso_file.seek((0x20 * iso_info['sector_size']) + self.iso_type['GDF'])
            if iso_file.read(20).decode("ascii", "ignore") == 'MICROSOFT*XBOX*MEDIA':
                iso_info['root_offset'] = self.iso_type['GDF']
            else:
                iso_file.seek((0x20 * iso_info['sector_size']) + self.iso_type['XGD3'])
                if iso_file.read(20).decode("ascii", "ignore") == 'MICROSOFT*XBOX*MEDIA':
                    iso_info['root_offset'] = self.iso_type['XGD3']
                else:
                    print('Unknown ISO format')
                    return False
        iso_file.seek((0x20 * iso_info['sector_size']) + iso_info['root_offset'])
        iso_info['identifier'] = iso_file.read(20).decode("ascii", "ignore")
        iso_info['root_dir_sector'] = unpack('I', iso_file.read(4))[0]
        iso_info['root_dir_size'] = unpack('I', iso_file.read(4))[0]
        iso_info['image_size'] = os.fstat(iso_file.fileno()).st_size
        iso_info['volume_size'] = iso_info['image_size'] - iso_info['root_offset']
        iso_info['volume_sectors'] = iso_info['volume_size'] / iso_info['sector_size']
        return iso_info

    @staticmethod
    def extract_defaultxex(iso_file, iso_info):
        # seek to root sector
        iso_file.seek((iso_info['root_dir_sector'] * iso_info['sector_size']) + iso_info['root_offset'])

        # read the root sector into a bytes object
        root_sector_buffer = io.BytesIO()
        root_sector_buffer.write(iso_file.read(iso_info['root_dir_size']))
        root_sector_buffer.seek(0)

        # case insensitive search of root sector for default.xex
        for i in range(0, iso_info['root_dir_size'] - 12):
            root_sector_buffer.seek(i)
            root_sector_buffer.read(1)  # file_attribute
            if int.from_bytes(root_sector_buffer.read(1), byteorder='big') == 11:  # 11 chars in filename
                if root_sector_buffer.read(11).decode("ascii", "ignore").lower() == 'default.xex':
                    # found default.xex
                    root_sector_buffer.seek(i - 8)
                    file_sector = unpack('I', root_sector_buffer.read(4))[0]
                    file_size = unpack('I', root_sector_buffer.read(4))[0]

                    # seek to default.xex
                    iso_file.seek(iso_info['root_offset'] + (file_sector * iso_info['sector_size']))

                    # read default.xex into a bytes object
                    xex_buffer = io.BytesIO()
                    xex_buffer.write(iso_file.read(file_size))
                    return xex_buffer
        print('default.xex not found')
        return False

    @staticmethod
    def extract_xex_info(xex_buffer):
        xex_info = {}

        xex_buffer.seek(0)
        if xex_buffer.read(4).decode("ascii", "ignore") == 'XEX2':

            # get the starting address of code from 0x08 in the xex
            xex_buffer.seek(0x08)
            code_offset = unpack('>I', xex_buffer.read(4))[0]
            # check if the code_offset is too large
            if code_offset > sys.getsizeof(xex_buffer):
                print('Starting address of Xex code is beyond size of default.xex')
                return False

            # get the starting address of the xex certificate
            xex_buffer.seek(0x10)
            cert_offset = unpack('>I', xex_buffer.read(4))[0]
            # check if the cert_offset is too large
            if cert_offset > code_offset:
                print('Xex certificate offset is beyond the starting address of Xex code')
                return False

            # get the number of entries in the general info table
            xex_buffer.seek(0x14)
            info_table_num_entries = unpack('>I', xex_buffer.read(4))[0]
            # check that there aren't too many entries
            if info_table_num_entries * 8 + 24 > code_offset:
                print('Xex general info table has entries that spill over into the Xex code')
                return False

            execution_info_address = False
            execution_info_table_flags = bytes([0x00, 0x04, 0x00, 0x06])

            # iterate through info table, finding addresses
            for i in range(0, info_table_num_entries):
                header_id = unpack('>I', xex_buffer.read(4))[0]

                if header_id == unpack('>I', execution_info_table_flags)[0]:
                    execution_info_address = unpack('>I', xex_buffer.read(4))[0]
                else:
                    xex_buffer.read(4)

            # seek to each address and extract info
            if execution_info_address is not False:
                xex_buffer.seek(execution_info_address)
                xex_info['media_id'] = binascii.hexlify(xex_buffer.read(4)).decode("ascii", "ignore").upper()
                xex_info['version'] = unpack('>I', xex_buffer.read(4))[0]
                xex_info['base_version'] = unpack('>I', xex_buffer.read(4))[0]
                xex_info['title_id'] = binascii.hexlify(xex_buffer.read(4)).decode("ascii", "ignore").upper()
                xex_info['platform'] = ord(xex_buffer.read(1))
                xex_info['executable_type'] = ord(xex_buffer.read(1))
                xex_info['disc_number'] = ord(xex_buffer.read(1))
                xex_info['disc_count'] = ord(xex_buffer.read(1))
            else:
                return False

            return xex_info
        else:
            print('XEX2 was not found at the start of default.xex')
            return False

    def media_id_to_game_name(self, media_id):
        # check if we've already loaded the csv
        if self.csv_file is None:
            if (self.csv_settings['force_update'] is True) or \
               (self.csv_exists() is False and self.csv_settings['download_if_missing'] is True):
                self.download_csv()

            if self.open_csv() is False:
                return False

            game_name = self.search_csv(media_id)
            if game_name is not None:
                return game_name
            elif game_name is None and os.stat(self.csv_settings['local']).st_mtime > self.csv_settings['min_age']:
                self.download_csv()
                if self.open_csv() is False:
                    return False
                return self.search_csv(media_id)

        return self.search_csv(media_id)

    def csv_exists(self):
        if os.path.isfile(self.csv_settings['local']):
            return True
        else:
            return False

    def download_csv(self):
        urllib.request.urlretrieve(self.csv_settings['url'], self.csv_settings['local'])

    def open_csv(self):
        if self.csv_exists():
            self.csv_file = open(self.csv_settings['local'], 'r')
        else:
            return False

    def search_csv(self, media_id):
        self.csv_file.seek(0)
        reader = csv.reader(self.csv_file, delimiter=',')
        for row in reader:
            for col in row:
                if col.endswith(media_id):
                    return row[0]
        return None
