#!/usr/bin/env python3
"""
MouseCross - App Store Connect API Helper
Assists with App Store Connect operations using the official API
Requires: requests, PyJWT libraries (pip install requests PyJWT)
"""

import json
import time
import base64
import requests
from datetime import datetime, timedelta
from pathlib import Path
import jwt
import argparse
import os
import sys

class AppStoreConnectAPI:
    """App Store Connect API client for MouseCross"""

    def __init__(self, key_id, issuer_id, private_key_path):
        """
        Initialize API client

        Args:
            key_id: Your API key ID from App Store Connect
            issuer_id: Your issuer ID from App Store Connect
            private_key_path: Path to your .p8 private key file
        """
        self.key_id = key_id
        self.issuer_id = issuer_id
        self.base_url = "https://api.appstoreconnect.apple.com"

        # Load private key
        with open(private_key_path, 'r') as f:
            self.private_key = f.read()

        self.token = self._generate_token()

    def _generate_token(self):
        """Generate JWT token for API authentication"""
        payload = {
            'iss': self.issuer_id,
            'iat': int(time.time()),
            'exp': int(time.time()) + 1200,  # 20 minutes
            'aud': 'appstoreconnect-v1'
        }

        headers = {
            'kid': self.key_id,
            'typ': 'JWT',
            'alg': 'ES256'
        }

        return jwt.encode(payload, self.private_key, algorithm='ES256', headers=headers)

    def _make_request(self, method, endpoint, data=None, params=None):
        """Make authenticated request to App Store Connect API"""
        url = f"{self.base_url}/v1/{endpoint}"
        headers = {
            'Authorization': f'Bearer {self.token}',
            'Content-Type': 'application/json'
        }

        response = requests.request(method, url, headers=headers, json=data, params=params)

        if response.status_code == 401:  # Token expired
            self.token = self._generate_token()
            headers['Authorization'] = f'Bearer {self.token}'
            response = requests.request(method, url, headers=headers, json=data, params=params)

        return response

    def get_apps(self):
        """Get all apps for the team"""
        response = self._make_request('GET', 'apps')
        if response.status_code == 200:
            return response.json()['data']
        else:
            print(f"Error fetching apps: {response.status_code}")
            print(response.text)
            return None

    def create_app(self, bundle_id, name, platform='MAC_OS', locale='en-US'):
        """
        Create a new app in App Store Connect
        Note: This requires the Account Holder role or Admin role with App Manager permissions
        """
        data = {
            'data': {
                'type': 'apps',
                'attributes': {
                    'bundleId': bundle_id,
                    'name': name,
                    'platform': platform,
                    'primaryLocale': locale
                }
            }
        }

        response = self._make_request('POST', 'apps', data)
        if response.status_code == 201:
            return response.json()['data']
        else:
            print(f"Error creating app: {response.status_code}")
            print(response.text)
            return None

    def get_app_info(self, bundle_id):
        """Get app information by bundle ID"""
        params = {'filter[bundleId]': bundle_id}
        response = self._make_request('GET', 'apps', params=params)

        if response.status_code == 200:
            data = response.json()['data']
            return data[0] if data else None
        else:
            print(f"Error fetching app info: {response.status_code}")
            print(response.text)
            return None

    def get_builds(self, app_id):
        """Get builds for an app"""
        endpoint = f'apps/{app_id}/builds'
        response = self._make_request('GET', endpoint)

        if response.status_code == 200:
            return response.json()['data']
        else:
            print(f"Error fetching builds: {response.status_code}")
            print(response.text)
            return None

    def update_app_info(self, app_id, **kwargs):
        """Update app information"""
        data = {
            'data': {
                'type': 'apps',
                'id': app_id,
                'attributes': {k: v for k, v in kwargs.items() if v is not None}
            }
        }

        response = self._make_request('PATCH', f'apps/{app_id}', data)
        if response.status_code == 200:
            return response.json()['data']
        else:
            print(f"Error updating app: {response.status_code}")
            print(response.text)
            return None

    def create_version(self, app_id, version_string, platform='MAC_OS'):
        """Create a new app version"""
        data = {
            'data': {
                'type': 'appStoreVersions',
                'attributes': {
                    'platform': platform,
                    'versionString': version_string
                },
                'relationships': {
                    'app': {
                        'data': {
                            'type': 'apps',
                            'id': app_id
                        }
                    }
                }
            }
        }

        response = self._make_request('POST', 'appStoreVersions', data)
        if response.status_code == 201:
            return response.json()['data']
        else:
            print(f"Error creating version: {response.status_code}")
            print(response.text)
            return None

def load_config():
    """Load configuration from environment or config file"""
    config = {}

    # Try to load from environment variables
    config['key_id'] = os.getenv('ASC_KEY_ID')
    config['issuer_id'] = os.getenv('ASC_ISSUER_ID')
    config['private_key_path'] = os.getenv('ASC_PRIVATE_KEY_PATH')

    # Try to load from config file
    config_file = Path('appstore_config.json')
    if config_file.exists():
        with open(config_file) as f:
            file_config = json.load(f)
            for key in ['key_id', 'issuer_id', 'private_key_path']:
                if not config.get(key):
                    config[key] = file_config.get(key)

    return config

def create_config_template():
    """Create a configuration template file"""
    template = {
        "key_id": "YOUR_API_KEY_ID",
        "issuer_id": "YOUR_ISSUER_ID",
        "private_key_path": "path/to/AuthKey_KEYID.p8",
        "bundle_id": "de.slohmaier.mousecross",
        "app_name": "MouseCross"
    }

    with open('appstore_config.json', 'w') as f:
        json.dump(template, f, indent=2)

    print("Created appstore_config.json template")
    print("Please edit it with your App Store Connect API credentials")
    print()
    print("To get API credentials:")
    print("1. Go to https://appstoreconnect.apple.com/access/api")
    print("2. Generate API key with App Manager role")
    print("3. Download the .p8 private key file")
    print("4. Copy Key ID and Issuer ID to the config file")

def main():
    parser = argparse.ArgumentParser(description='MouseCross App Store Connect API Helper')
    parser.add_argument('--setup', action='store_true', help='Create configuration template')
    parser.add_argument('--list-apps', action='store_true', help='List all apps')
    parser.add_argument('--app-info', help='Get app info by bundle ID')
    parser.add_argument('--create-app', action='store_true', help='Create MouseCross app')
    parser.add_argument('--list-builds', help='List builds for app ID')
    parser.add_argument('--create-version', help='Create new version for app ID')

    args = parser.parse_args()

    if args.setup:
        create_config_template()
        return

    # Load configuration
    config = load_config()

    if not all(config.get(key) for key in ['key_id', 'issuer_id', 'private_key_path']):
        print("Missing required configuration. Run with --setup to create template.")
        print("Required: key_id, issuer_id, private_key_path")
        return

    try:
        api = AppStoreConnectAPI(
            config['key_id'],
            config['issuer_id'],
            config['private_key_path']
        )
    except Exception as e:
        print(f"Error initializing API client: {e}")
        return

    if args.list_apps:
        print("Fetching apps...")
        apps = api.get_apps()
        if apps:
            for app in apps:
                print(f"ID: {app['id']}")
                print(f"Name: {app['attributes']['name']}")
                print(f"Bundle ID: {app['attributes']['bundleId']}")
                print(f"Platform: {app['attributes']['platform']}")
                print("---")

    elif args.app_info:
        print(f"Fetching app info for {args.app_info}...")
        app = api.get_app_info(args.app_info)
        if app:
            print(json.dumps(app, indent=2))
        else:
            print("App not found")

    elif args.create_app:
        bundle_id = config.get('bundle_id', 'de.slohmaier.mousecross')
        app_name = config.get('app_name', 'MouseCross')

        print(f"Creating app: {app_name} ({bundle_id})")
        app = api.create_app(bundle_id, app_name)
        if app:
            print("App created successfully!")
            print(f"App ID: {app['id']}")
        else:
            print("Failed to create app")

    elif args.list_builds:
        print(f"Fetching builds for app {args.list_builds}...")
        builds = api.get_builds(args.list_builds)
        if builds:
            for build in builds:
                print(f"Version: {build['attributes']['version']}")
                print(f"Build: {build['attributes']['buildNumber']}")
                print(f"State: {build['attributes']['processingState']}")
                print("---")

    elif args.create_version:
        app_id = args.create_version
        version = "0.1.0"  # Default version

        print(f"Creating version {version} for app {app_id}...")
        result = api.create_version(app_id, version)
        if result:
            print("Version created successfully!")
            print(f"Version ID: {result['id']}")
        else:
            print("Failed to create version")

    else:
        parser.print_help()

if __name__ == '__main__':
    main()